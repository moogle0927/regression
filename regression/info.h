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
	
	int n; //症例数
	int p; //軸数
	int num_p; //見たい主成分スコアの軸番号

	inline void input(const std::string &path)
	{
		nari::infocontroller info;
		info.load(path);
		dir_shape = nari::file::add_delim(info.get_as_str("dir_shape"));
		dir_def = nari::file::add_delim(info.get_as_str("dir_def"));
		dir_out = nari::file::add_delim(info.get_as_str("dir_out"));
		
		n = info.get_as_int("n_case");
		p = info.get_as_int("P");
		num_p = info.get_as_int("P_num");

	}

};
#endif