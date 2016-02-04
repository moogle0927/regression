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
	
	int n; //症例数
	int p; //軸数
	int s; //軸数
	int d; //軸数
	int n_num;
	//int num_p; //見たい主成分スコアの軸番号

	inline void input(const std::string &path)
	{
		nari::infocontroller info;
		info.load(path);
		dir_shape = nari::file::add_delim(info.get_as_str("dir_shape"));
		dir_def = nari::file::add_delim(info.get_as_str("dir_def"));
		dir_out = nari::file::add_delim(info.get_as_str("dir_out"));
		dir_test = nari::file::add_delim(info.get_as_str("dir_shape")+"test/");

		n = info.get_as_int("n_case");
		p = info.get_as_int("P"); 
		s = info.get_as_int("S"); //説明変数の数
		d = info.get_as_int("D"); //目的変数の数
		n_num = info.get_as_int("n_num")-1;
		//num_p = info.get_as_int("P_num");

	}

};
#endif