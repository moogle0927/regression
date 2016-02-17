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
#include "direct.h"
#include <naricommon.h>
#include <nariinfocontroller.h>
#include <narifile.h>

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

	std::vector<int> s;
	std::vector<int> d;
	std::ifstream s_path(input_info.dir_s);
	std::ifstream d_path(input_info.dir_d);
	std::string buf_s;
	std::string buf_d;
	while (s_path&& getline(s_path, buf_s))
	{
		s.push_back(std::stoi(buf_s));
	}
	while (d_path&& getline(d_path, buf_d))
	{
		d.push_back(std::stoi(buf_d));
	}
	
	

	//ファイル読み込み
	//形状主成分スコア
	std::vector<double> shape;
	read_vector(shape, input_info.dir_shape + "mat.raw");

	// テスト症例形状主成分スコア
	std::vector<double> test;
	read_vector(test, input_info.dir_test + "mat.raw");

	//変形場主成分スコア
	std::vector<double> deformation;
	read_vector(deformation, input_info.dir_def + "mat.raw");

	//変形場主成分スコア
	std::vector<double> deftest;
	read_vector(deftest, input_info.dir_deftest + "mat.raw");


	//入力した軸の数でループ
	for (int j = 0; j < s.size(); j++) {
		for (int k = 0; k < d.size(); k++) {

			//読み込んだデータで行列を作る
			Eigen::MatrixXd Shape_read_0 = Eigen::Map<Eigen::MatrixXd>(&shape[0], input_info.p, input_info.n);
			Eigen::MatrixXd Shape_read = Shape_read_0.block(0, 0, s[j], input_info.n);
			Eigen::MatrixXd Deformation_read_0 = Eigen::Map<Eigen::MatrixXd>(&deformation[0], input_info.p, input_info.n);
			Eigen::MatrixXd Deformation_read = Deformation_read_0.block(0, 0, d[k], input_info.n);
			Eigen::MatrixXd Deformation = Deformation_read.transpose();
			Eigen::MatrixXd Test_read_0 = Eigen::Map<Eigen::MatrixXd>(&test[0], input_info.p, input_info.n + 1);
			Eigen::MatrixXd Test_read = Test_read_0.block(0, 0, s[j], input_info.n + 1);
			Eigen::MatrixXd dTest_read_0 = Eigen::Map<Eigen::MatrixXd>(&deftest[0], input_info.p, input_info.n + 1);
			Eigen::MatrixXd dTest = dTest_read_0.block(0, 0, d[k], input_info.n + 1);
			//一番左の列に１をいれる
			Eigen::MatrixXd Shape = Shape.Ones(input_info.n, s[j] + 1);
			Shape.block(0, 1, input_info.n, s[j]) = Shape_read.transpose();
			//テストデータ一番左に１をいれる
			Eigen::MatrixXd Test = Test.Ones(input_info.n + 1, s[j] + 1);
			Test.block(0, 1, input_info.n + 1, s[j]) = Test_read.transpose();

			std::cout << "shape" << std::endl << Shape << std::endl;
			std::cout << "def" << std::endl << Deformation << std::endl;
			std::stringstream dir_Reg;
			dir_Reg << input_info.dir_out << "D" << d[k] << "S" << s[j] << ".csv";
			std::ofstream mat_Reg(dir_Reg.str());
			Eigen::MatrixXd Result;
			Result = Result.Zero(d[k], 1);
			Eigen::MatrixXd Dif;
			Dif = Dif.Zero(d[k], input_info.n + 1);

			for (int a = 0; a < d[k]; a++) {
				/*for (int i = 0; i < Deformation.block(0, a, input_info.n, 1).rows(); i++) {
				for (int j = 0; j < Deformation.block(0, a, input_info.n, 1).cols(); j++) {

				mat_def << Deformation.block(0, a, input_info.n, 1)(i, j) << ",";
				}
				mat_def << std::endl;
				}*/

				//係数を算出
				Eigen::MatrixXd beta_0 = Deformation.block(0, a, input_info.n, 1);
				Eigen::MatrixXd beta_1 = Shape.transpose()*Shape;
				Eigen::MatrixXd beta = beta_1.inverse()*Shape.transpose()*beta_0;

				//回帰モデルから算出したスコア
				Eigen::MatrixXd reg_0 = Test*beta;
				Eigen::MatrixXd reg = reg_0.block(input_info.n_num, 0, 1, 1);
				Eigen::MatrixXd dif = reg_0.transpose();
				std::cout << "(^^)" << std::endl;
				Result.block(a, 0, 1, 1) = reg;
				Dif.block(a, 0, 1, input_info.n + 1) = dif;
				std::cout << "(^^)" << std::endl;
				std::cout << Result << std::endl;

				std::stringstream dirOUT;
				dirOUT << input_info.dir_out << "D" << d[k] << "S" << s[j] << "/" << a << "beta";
				nari::system::make_directry(dirOUT.str());
				write_matrix_raw_and_txt(beta, dirOUT.str());
				std::ofstream mat_beta(dirOUT.str() + ".csv");


				for (int i = 0; i < beta.rows(); i++) {

					mat_beta << beta(i, 0) << std::endl;
				}



			}

			std::stringstream dirOUT_reg;
			dirOUT_reg << input_info.dir_out << "D" << d[k] << "S" << s[j] << "/" << "test";
			nari::system::make_directry(dirOUT_reg.str());
			write_matrix_raw_and_txt(Result, dirOUT_reg.str());
			std::ofstream mat_reg(dirOUT_reg.str() + ".csv");
			for (int i = 0; i < Result.rows(); i++) {
				mat_Reg << Result(i, 0) << std::endl;
			}

			std::stringstream dirOUT_dif;
			dirOUT_dif << input_info.dir_out << "D" << d[k] << "S" << s[j] << "/" << "dif";
			nari::system::make_directry(dirOUT_dif.str());
			write_matrix_raw_and_txt(Dif, dirOUT_dif.str());
			std::ofstream mat_dif(dirOUT_dif.str() + ".csv");
			Eigen::MatrixXd diff = Dif - dTest;
			Eigen::MatrixXd diff_2 = diff_2.Zero(d[k], input_info.n + 1);
			
			for (int i = 0; i < Dif.rows(); i++) {
				for (int j = 0; j < Dif.cols(); j++) {
					Eigen::MatrixXd aaaa = diff.block(i, j, 1, 1)*diff.block(i, j, 1, 1);
					mat_dif << aaaa << ",";
				}
				mat_dif << std::endl;
			}
		}
	}

	

}