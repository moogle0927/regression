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
	// W�̏����o��												//
	// row���B��w�̐��Ccol�����w�̐�							//			
	// �d�݂̉������s���ꍇ�́C�e�s��؂�o����reshape���s��  //
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

	//�t�@�C���ǂݍ���
	//�`��听���X�R�A
	std::vector<double> shape;
	read_vector(shape, input_info.dir_shape + "mat.raw");

	// �e�X�g�Ǘ�`��听���X�R�A
	std::vector<double> test;
	read_vector(test, input_info.dir_test + "mat.raw");

	//�ό`��听���X�R�A
	std::vector<double> deformation;
	read_vector(deformation, input_info.dir_def + "mat.raw");

	//�ǂݍ��񂾃f�[�^�ōs������
	Eigen::MatrixXd Shape_read = Eigen::Map<Eigen::MatrixXd>(&shape[0], input_info.p, input_info.n);
	Eigen::MatrixXd Deformation_read = Eigen::Map<Eigen::MatrixXd>(&deformation[0], input_info.p, input_info.n);
	Eigen::MatrixXd Deformation = Deformation_read.transpose();
	Eigen::MatrixXd Test_read = Eigen::Map<Eigen::MatrixXd>(&test[0], input_info.p, input_info.n+1);
	//��ԍ��̗�ɂP�������
	Eigen::MatrixXd Shape = Shape.Ones(input_info.n, input_info.p + 1);
	Shape.block(0, 1, input_info.n, input_info.p) = Shape_read.transpose();
	//�e�X�g�f�[�^��ԍ��ɂP�������
	Eigen::MatrixXd Test = Test.Ones(input_info.n+1, input_info.p + 1);
	Test.block(0, 1, input_info.n+1, input_info.p) = Test_read.transpose();

	std::cout << "shape"<<std::endl<<Shape << std::endl;
	std::cout << "def" << std::endl << Deformation << std::endl;
	std::ofstream mat_Reg(input_info.dir_out + "result.csv");
	Eigen::MatrixXd Result;
	Result = Result.Zero(input_info.p, 1);

	for (int a = 0; a < input_info.p; a++) {
		/*for (int i = 0; i < Deformation.block(0, a, input_info.n, 1).rows(); i++) {
			for (int j = 0; j < Deformation.block(0, a, input_info.n, 1).cols(); j++) {

				mat_def << Deformation.block(0, a, input_info.n, 1)(i, j) << ",";
			}
			mat_def << std::endl;
		}*/

		//�W�����Z�o
		Eigen::MatrixXd beta_0 = Deformation.block(0, a, input_info.n, 1);
		Eigen::MatrixXd beta_1 = Shape.transpose()*Shape;
		Eigen::MatrixXd beta = beta_1.inverse()*Shape.transpose()*beta_0;

		//��A���f������Z�o�����X�R�A
		Eigen::MatrixXd reg_0 = Test*beta;
		Eigen::MatrixXd reg = reg_0.block(input_info.n_num, 0, 1, 1);
		std::cout << "(^^)" << std::endl;
		Result.block(a,0,1,1)=reg;
		std::cout << "(^^)" << std::endl;
		std::cout << Result << std::endl;
		
		std::stringstream dirOUT;
		dirOUT << input_info.dir_out << a << "beta";
		write_matrix_raw_and_txt(beta, dirOUT.str());
		std::ofstream mat_beta(dirOUT.str()+".csv");


		for (int i = 0; i < beta.rows(); i++) {

			mat_beta << beta(i, 0) << std::endl;
		}
		
		
		
	}

	std::stringstream dirOUT_reg;
	dirOUT_reg << input_info.dir_out <<  "test";
	write_matrix_raw_and_txt(Result, dirOUT_reg.str());
	std::ofstream mat_reg(dirOUT_reg.str() + ".csv");
	for (int i = 0; i < Result.rows(); i++) {
		mat_Reg << Result(i, 0) << std::endl;
	}

}