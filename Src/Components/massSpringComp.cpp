#include "massSpringComp.h"

MassSpring::MassSpring(std::shared_ptr<TetMesh> _tetMesh)
{
	tetMesh = _tetMesh;
}

void MassSpring::Start()
{
	std::cout << "\nMassSpring Start" << std::endl;
	// Find all unique springs
	
	std::unordered_set<std::pair<uint32_t, uint32_t>, CustomUtils::PairHash> uniqueSprings;
	std::pair<uint32_t,uint32_t> currSpring;
	for (uint32_t i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	{
		for (uint32_t j = 0; j < 4; j++)
		{
			for (uint32_t k = j + 1; k < 4; k++)
			{
				currSpring.first = tetMesh->tetData.tetrahedra(i,j);
				currSpring.second = tetMesh->tetData.tetrahedra(i,k);
				if (currSpring.first > currSpring.second)
					std::swap(currSpring.first, currSpring.second);
				uniqueSprings.insert(currSpring);
			}
		}
	}
	std::cout<<"\nNumber of unique springs: "<<uniqueSprings.size()<<std::endl;

	// Compute rest lengths
	for (std::pair<uint32_t, uint32_t> curSpr : uniqueSprings)
	{
		springs.emplace_back(curSpr);
		restLengths.emplace_back((tetMesh->tetData.vertices.segment<3>(3 * curSpr.first) - tetMesh->tetData.vertices.segment<3>(3* curSpr.second)).norm());
		auto test = (tetMesh->tetData.vertices.segment<3>(3 * curSpr.first) - tetMesh->tetData.vertices.segment<3>(3 * curSpr.second)).norm();
		assert(restLengths.back() > 0.0);
	}

	positions = tetMesh->tetData.vertices;
	velocity.resizeLike(positions);
	velocity.setZero();
	force.resizeLike(positions);

	this->massInvMatrix.resize(positions.size(), positions.size());
	this->massInvMatrix.setZero();
	this->massInvMatrix.reserve(Eigen::VectorXi::Constant(positions.size(), 1));
	calculateMassMatrix();
}

void MassSpring::fixedUpdate(float dt)
{
	calculateForces();

}

void MassSpring::calculateForces()
{
	//CustomUtils::Stopwatch sw("calculateForces");
	// Iterate through all vertices and apply gravity
	#pragma omp parallel for
	for (int i = 0; i < positions.size() / 3; i++)
	{		
		this->force.segment<3>(3 * i) = Eigen::Vector3f(0.0, -9.8, 0.0);
	}

	// Iterate through all springs and apply spring forces
	#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3f springForces = Eigen::Vector3f::Zero();
		Eigen::Vector3f springVector = positions.segment<3>(3 * springs[i].second) - positions.segment<3>(3 * springs[i].first);
		float springLength = springVector.norm();
		assert(springLength > 0.0);
		springVector.normalize();
		// Compute spring force
		springForces = (this->springStiffness / this->restLengths[i]) * (springLength - restLengths[i]) * springVector;

		// Compute damping force
		Eigen::Vector3f velocityDifference = this->velocity.segment<3>(3 * springs[i].second) - this->velocity.segment<3>(3 * springs[i].first);
		springForces += this->damping *  velocityDifference.dot(springVector) * springVector;
		for (int j = 0; j < 3; j++)
		{
			#pragma omp atomic
			this->force(3 * springs[i].first + j) += springForces(j);
			#pragma omp atomic
			this->force(3 * springs[i].second + j) -= springForces(j);
		}		
	}
}


// iterate through each tetrahedron and assign weights to each vertex based on the tetrahedron volume
void MassSpring::calculateMassMatrix()
{
	CustomUtils::Stopwatch sw("calculateMassMatrix");

	// find total volume of the mesh
	float totalVolume = 0.0f;
	std::vector<float> tetVolumes(tetMesh->tetData.tetrahedra.rows());
	Eigen::VectorXf massDiagonal(this->positions.size() / 3);
	massDiagonal.setZero();
	#pragma omp parallel for
	for (int i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	{
		Eigen::Vector3f v0, v1, v2, v3;
		v0 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 0));
		v1 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 1));
		v2 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 2));
		v3 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 3));
		// use Cayley-Menger determinant to find volume
		Eigen::Matrix<float,5,5> cayleyMenger;
		cayleyMenger << 0, 1, 1, 1, 1,
			1, 0, (v0 - v1).squaredNorm(), (v0 - v2).squaredNorm(), (v0 - v3).squaredNorm(),
			1, (v0 - v1).squaredNorm(), 0, (v1 - v2).squaredNorm(), (v1 - v3).squaredNorm(),
			1, (v0 - v2).squaredNorm(), (v1 - v2).squaredNorm(), 0, (v2 - v3).squaredNorm(),
			1, (v0 - v3).squaredNorm(), (v1 - v3).squaredNorm(), (v2 - v3).squaredNorm(), 0;
		float volume = sqrt(cayleyMenger.determinant() / 288.0);
		assert(volume > 0.0f && std::isnan(volume) == false);
		tetVolumes[i] = volume;
		#pragma omp atomic
		totalVolume += volume;
	}

	// iterate through each tetrahedron and assign weights to each vertex based on the tetrahedron volume
	#pragma omp parallel for
	for (int i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			#pragma omp atomic
			massDiagonal(tetMesh->tetData.tetrahedra(i, j)) += this->totalMass * tetVolumes[i] / totalVolume;
		}
	}

	#pragma omp parallel for
	for (int i = 0; i < massDiagonal.size(); i++)	
		massDiagonal(i) /= 4;

	assert(CustomUtils::epsEqual(massDiagonal.sum(), this->totalMass, 3.0f));
	
	// fill mass inverse matrix with the diagonal entries
	for (int i = 0; i < massDiagonal.size(); i++)
	{
		float invMass = 1.0f / massDiagonal(i);
		for (int j = 0; j < 3; j++)
		{
			this->massInvMatrix.insert(3 * i + j, 3 * i + j) = invMass;
		}
	}
	massInvMatrix.makeCompressed();
}

MassSpring::~MassSpring()
{
}