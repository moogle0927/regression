#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
//#include "Eigen\core"
#include "narivectorpp.h"
#include "info.h"
#include <Eigen/Dense>
#include <sys/stat.h>

template< class T >
void write_vector(std::vector<T> &v, const std::string filename) {
	FILE *fp;
	if (fopen_s(&fp, filename.c_str(), "wb") != 0) {
		std::cerr << "Cannot open file: " << filename << std::endl;
		std::abort();
	}
	fwrite(v.data(), sizeof(T), v.size(), fp);
	fclose(fp);
}


long get_file_size(std::string filename)
{
	FILE *fp;
	struct stat st;
	if (fopen_s(&fp, filename.c_str(), "rb") != 0) {
		std::cerr << "Cannot open file: " << filename << std::endl;
		std::abort();
	}
	fstat(_fileno(fp), &st);
	fclose(fp);
	return st.st_size;
}


template< class T >
void read_vector(std::vector<T> &v, const std::string filename) {

	auto num = get_file_size(filename) / sizeof(T);
	FILE *fp;
	if (fopen_s(&fp, filename.c_str(), "rb") != 0) {
		std::cerr << "Cannot open file: " << filename << std::endl;
		std::abort();
	}
	v.resize(num);
	fread(v.data(), sizeof(T), num, fp);
	fclose(fp);
}



template<typename T>
void write_matrix_raw_and_txt(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& data, std::string filename)
{
	//////////////////////////////////////////////////////////////
	// Wの書き出し												//
	// rowが隠れ層の数，colが可視層の数							//			
	// 重みの可視化を行う場合は，各行を切り出してreshapeを行う  //
	//////////////////////////////////////////////////////////////
	size_t rows = data.rows();
	size_t cols = data.cols();
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> Data;
	Data = data;
	std::ofstream fs1(filename + ".txt");
	fs1 << "rows = " << rows << std::endl;
	fs1 << "cols = " << cols << std::endl;
	fs1 << typeid(Data).name() << std::endl;
	fs1.close();
	std::vector<T> save_data(rows * cols);
	Data.resize(rows * cols, 1);
	for (size_t i = 0; i <save_data.size(); i++)
		save_data[i] = Data(i, 0);
	write_vector(save_data, filename + ".raw");
	Data.resize(rows, cols);
}

void main(int argc, char *argv[]) {
	info input_info;
	input_info.input(argv[1]);

	//ファイル読み込み
	//形状主成分スコア
	std::vector<double> shape;
	read_vector(shape, input_info.dir_shape + "mat.raw");

	//変形場主成分スコア
	std::vector<double> deformation;
	read_vector(deformation, input_info.dir_def + "mat.raw");

	//読み込んだデータで行列を作る
	Eigen::MatrixXd Shape_read = Eigen::Map<Eigen::MatrixXd>(&shape[0], input_info.p, input_info.n);
	Eigen::MatrixXd Deformation_read = Eigen::Map<Eigen::MatrixXd>(&deformation[0], input_info.p, input_info.n);
	Eigen::MatrixXd Deformation = Deformation_read.transpose();
	
	//一番左の列に１をいれる
	Eigen::MatrixXd Shape = Shape.Ones(input_info.n, input_info.p + 1);
	Shape.block(0, 1, input_info.n, input_info.p) = Shape_read.transpose();


	std::ofstream mat_def(input_info.dir_out + "deformation.csv");

	for (int i = 0; i < Deformation.block(0, input_info.num_p - 1, input_info.n, 1).rows(); i++) {
		for (int j = 0; j < Deformation.block(0, input_info.num_p - 1, input_info.n, 1).cols(); j++) {

			mat_def << Deformation.block(0, input_info.num_p - 1, input_info.n, 1)(i, j) << ",";
		}
		mat_def  << std::endl;
	}

	//係数を算出
	Eigen::MatrixXd beta_0 = Deformation.block(0, input_info.num_p - 1, input_info.n, 1);
	Eigen::MatrixXd beta_1 = Shape.transpose()*Shape;
	Eigen::MatrixXd beta = beta_1.inverse()*Shape.transpose()*beta_0;

	
	write_matrix_raw_and_txt(beta, input_info.dir_out+"beta");
	std::ofstream mat_beta(input_info.dir_out + "beta.csv");

	for (int i = 0; i < beta.rows(); i++) {

		mat_beta << beta(i, 0) << std::endl;
	}

}