#pragma once
#ifndef __HIST_INFO__
#define __HIST_INFO__
#include<iostream>
#include <string>
#include <algorithm>
#include <vector>

#include <kawamhd.h>

#include <naricommon.h>
#include <nariinfocontroller.h>
#include <narifile.h>

struct info
{
	std::string dir_shape;
	std::string dir_def;
	std::string dir_out;
	std::string dir_test;
	std::string dir_deftest;
	std::string dir_s;
	std::string dir_d;
	
	int n; //�Ǘᐔ
	int p; //����

	int n_num;
	//int num_p; //�������听���X�R�A�̎��ԍ�

	inline void input(const std::string &path)
	{
		nari::infocontroller info;
		info.load(path);
		dir_shape = nari::file::add_delim(info.get_as_str("dir_shape"));
		dir_def = nari::file::add_delim(info.get_as_str("dir_def"));
		dir_out = nari::file::add_delim(info.get_as_str("dir_out"));
		dir_test = nari::file::add_delim(info.get_as_str("dir_shape")+"test/");
		dir_deftest = nari::file::add_delim(info.get_as_str("dir_def") + "test/");

		n = info.get_as_int("n_case");
		p = info.get_as_int("P"); 
		dir_s = info.get_as_str("S"); //�����ϐ��̐�
		dir_d = info.get_as_str("D"); //�ړI�ϐ��̐�

		n_num = info.get_as_int("n_num")-1;
		//num_p = info.get_as_int("P_num");

	}

};
#endif