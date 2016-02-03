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
	
	int n; //Ç—á”
	int p; //²”
	int n_num;
	//int num_p; //Œ©‚½‚¢å¬•ªƒXƒRƒA‚Ì²”Ô†

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
		n_num = info.get_as_int("n_num")-1;
		//num_p = info.get_as_int("P_num");

	}

};
#endif