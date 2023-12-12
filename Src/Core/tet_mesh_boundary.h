#pragma once

# include <cstdlib>
# include <iostream>
# include <fstream>
# include <iomanip>
# include <cmath>
# include <ctime>
# include <string>
# include <unordered_map>


int loadTetMesh(std::string prefix);
char ch_cap(char ch);
bool ch_eqi(char ch1, char ch2);
int ch_to_digit(char ch);
int file_column_count(std::string input_filename);
int file_row_count(std::string input_filename);
int i4_max(int i1, int i2);
int i4_min(int i1, int i2);
int i4col_compare(int m, int n, int a[], int i, int j);
void i4col_sort_a(int m, int n, int a[]);
void i4col_swap(int m, int n, int a[], int icol1, int icol2);
void i4i4i4_sort_a(int i1, int i2, int i3, int* j1, int* j2, int* j3);
int* i4mat_data_read(std::string input_filename, int m, int n);
void i4mat_header_read(std::string input_filename, int* m, int* n);
void i4mat_transpose_print_some(int m, int n, int a[], int ilo, int jlo,
	int ihi, int jhi, std::string title);
void i4mat_write(std::string output_filename, int m, int n, int table[]);
int* i4vec_cum(int n, int a[]);
int i4vec_sum(int n, int a[]);
void mesh_base_zero(int node_num, int element_order, int element_num,
	int element_node[]);
double* r8mat_data_read(std::string input_filename, int m, int n);
void r8mat_header_read(std::string input_filename, int* m, int* n);
void r8mat_transpose_print_some(int m, int n, double a[], int ilo, int jlo,
	int ihi, int jhi, std::string title);
void r8mat_write(std::string output_filename, int m, int n, double table[]);
int s_len_trim(std::string s);
int s_to_i4(std::string s, int* last, bool* error);
bool s_to_i4vec(std::string s, int n, int ivec[]);
double s_to_r8(std::string s, int* lchar, bool* error);
bool s_to_r8vec(std::string s, int n, double rvec[]);
int s_word_count(std::string s);
void sort_heap_external(int n, int* indx, int* i, int* j, int isgn);
void tet_mesh_boundary_count(int element_order, int element_num,
	int element_node[], int node_num, int* boundary_node_num,
	int* boundary_element_num, int boundary_node_mask[]);
int* tet_mesh_boundary_set(int element_order, int element_num,
	int element_node[], int boundary_element_order, int boundary_element_num);
void timestamp();

