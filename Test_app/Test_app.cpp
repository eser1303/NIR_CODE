#include "stdafx.h"

using namespace cv;
using namespace std;

vector<string> getNamesOfFile(string &folderPath)
{
	// vector of names of file within a folder 
	vector<string> names_ofFiles;

	char search[200];
	// convert your string search path into char array
	sprintf(search, "%s*.bmp*", folderPath.c_str());

	//  WIN32_FIND_DATA is a structure
	// Contains information about the file that is found by the FindFirstFile
	WIN32_FIND_DATA Info;
	// Find first file inside search path
	HANDLE Find = ::FindFirstFile(search, &Info);

	// If  FindFirstFile(search, &fd); fails return INVALID_HANDLE_VALUE,
	// If Find is not equal to INVALID_HANDLE_VALUE do the loop inside the
	// IF condition

	if (Find != INVALID_HANDLE_VALUE) {
		do {

			// DWORD dwFileAttributes , FILE_ATTRIBUTE_DIRECTORY ident a directory
			// If condition is fine lets push file mane into string vector
			if (!(Info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				// Fill vector of names
				names_ofFiles.push_back(Info.cFileName);

			}

			// do - while there is next file
			//::FindNextFile the function succeeds, the return value is nonzero and the Info contain //information about next file

		} while (::FindNextFile(Find, &Info));
		::FindClose(Find);
	}

	// Return result
	cout << names_ofFiles.size() << endl;
	return names_ofFiles;
}

pair<string,vector<int>> spliter(string &str)
{
	istringstream ss(str);
	vector<int> numbs;
	string name;
	int numb;
	ss >> name;
	while (ss) {	
		ss >> numb;
		numbs.push_back(numb);
	}
	return pair<string, vector<int>>(name, numbs);
}

pair<string, vector<int>> spliter_resf(string &str)
{
	istringstream ss(str);
	vector<int> numbs;
	string name;
	int numb;
	ss >> name;
	for (int i = 0 ; i < 5; ++i)
	{
		ss >> numb;
		numbs.push_back(numb);
	}
	return pair<string, vector<int>>(name, numbs);
}

int grad_bin(Mat &img, Mat &vert, Mat &horiz, int por)
{
	for (int i = 0; i < img.rows - 1; ++i)
	{
		for (int j = 0; j < img.cols - 1; ++j)
		{
			int der = (img.at<uchar>(i + 1, j) - img.at<uchar>(i, j)) / 2;
			if (abs(der) > por)
			{
				vert.at<uchar>(i, j) = 255;
			}
			else
			{
				vert.at<uchar>(i, j) = 0;
			}
			//dif.at<uchar>(i, j) = der;
			der = (img.at<uchar>(i, j + 1) - img.at<uchar>(i, j)) / 2;
			if (abs(der) > por)
			{
				horiz.at<uchar>(i, j) = 255;
			}
			else
			{
				horiz.at<uchar>(i, j) = 0;
			}
		}
	}
	return 0;
}

int interpol(Mat &img, Mat &rez, string dir)
{
	if (dir == "horiz")
	{
		for (int i = 1; i < img.rows - 1; ++i)
		{
			for (int j = 0; j < img.cols; ++j)
			{
				if (img.at<uchar>(i + 1, j) + img.at<uchar>(i - 1, j) + img.at<uchar>(i, j) == 255 * 3)
				{
					rez.at<uchar>(i, j) = 255;
				}
				else
				{
					rez.at<uchar>(i, j) = 0;
				}
			}
		}
	}
	if (dir == "vert")
	{
		for (int i = 0; i < img.rows; ++i)
		{
			for (int j = 1; j < img.cols - 1; ++j)
			{
				if (img.at<uchar>(i, j + 1) + img.at<uchar>(i, j - 1) + img.at<uchar>(i, j) == 255 * 3)
				{
					rez.at<uchar>(i, j) = 255;
				}
				else
				{
					rez.at<uchar>(i, j) = 0;
				}
			}
		}
	}
	return 0;
}

int max_function(Mat &img, Mat &max_a_b_pict,int &max_a_b_sum, int &max_a_b_a
	, int &max_a_b_b, int &max_a_b_i, int &min_a_b_y, int &min_a_b_y_2, int &a_b_move, int a_it, int b_it, int min_y, int min_y_2, int move, int &max_move)
{
	int row_sum;
	int max_sum = 0;
	int max_sum_i = 0;
	for (int i = 0; i < img.rows; ++i)
	{
		row_sum = 0;
		for (int j = 0; j < img.cols; ++j)
		{
			row_sum += img.at<uchar>(i, j);
		}
		if (row_sum >= max_sum)
		{
			max_sum_i = i;
			max_sum = row_sum;
		}

	}

	if (max_a_b_sum < max_sum)
	{
		max_a_b_sum = max_sum;
		max_a_b_a = a_it;
		max_a_b_b = b_it;
		max_a_b_i = max_sum_i;
		min_a_b_y = min_y;
		min_a_b_y_2 = min_y_2;
		a_b_move = move;
		max_move = max_sum_i + min_y + min_y_2 - move;
		img.copyTo(max_a_b_pict);
	}
	//cout << max_sum << " " << a_it << " " << b_it << endl;
	return 0;
}

int b_search_r(int b_it_min, int b_it_max, int b_it_step,
	Mat &img, Mat &max_a_b_pict 
	, int &max_a_b_sum, int &max_a_b_a
	, int &max_a_b_b, int &max_a_b_i, int &min_a_b_y, int &min_a_b_y_2, int &a_b_move, int a_it, int min_y,
	int &min_y_2, int move, int &max_move)
{
	for (int b_it = b_it_min; b_it < b_it_max; b_it += b_it_step)
	{
		double b = static_cast<double>(a_it * b_it * 2) / 1048576;//0.0019;
		Mat A_refresh_2(int(round(img.rows + b*img.cols)), img.cols, CV_8UC1, Scalar(0));

		min_y_2 = static_cast<int>(floor(img.rows + b*img.cols));
		int max_y_2 = 0;

		for (int i = 0; i < img.rows; ++i)
		{
			for (int j = 0; j < img.cols; ++j)
			{
				int y = static_cast<int>(floor(i + b*j));
				A_refresh_2.at<uchar>(y, j) = img.at<uchar>(i, j);
				if (A_refresh_2.at<uchar>(y, j) == 255)
				{
					if (y < min_y_2)
					{
						min_y_2 = y;
					}
					if (y > max_y_2)
					{
						max_y_2 = y;
					}
				}
			}
		}

		//resize(rez, rez, Size((max_y_2 - min_y_2), A_refresh_2.cols));
		Mat A_refresh_cut_2((max_y_2 - min_y_2), A_refresh_2.cols, CV_8UC1, Scalar(0));

		for (int i = 0; i < A_refresh_cut_2.rows; ++i)
		{
			for (int j = 0; j < A_refresh_cut_2.cols; ++j)
			{
				A_refresh_cut_2.at<uchar>(i, j) = A_refresh_2.at<uchar>(i + min_y_2, j);
			}
		}
		//imwrite("result.jpg", A_refresh_cut_2);
		max_function(A_refresh_cut_2, max_a_b_pict, max_a_b_sum, max_a_b_a
			, max_a_b_b, max_a_b_i, min_a_b_y, min_a_b_y_2, a_b_move, a_it, b_it, min_y, min_y_2, move, max_move);
	}
	return 0;
}

int a_search_r(int a_it_min, int a_it_max, int a_it_step, int b_it_min , int b_it_max, int b_it_step, 
			Mat &img, Mat &max_a_b_pict, int &max_a_b_sum, int &max_a_b_a, int &max_a_b_b, int &max_a_b_i, int &min_a_b_y, int &min_a_b_y_2, int &a_b_move, int &max_move)
{
	for (int a_it = a_it_min; a_it < a_it_max; a_it+=a_it_step)
	{
		double a = static_cast<double>(a_it) / 1048576;
		int y;
		int move = static_cast<int>(floor(a*img.cols*img.cols));
		int min_y = img.cols + move;
		int max_y = 0;

		Mat A_refresh(img.cols + move, img.cols, CV_8UC1, Scalar(0));
		//cout << "min_y: " << min_y << endl;
		for (int i = 0; i < img.rows; ++i)
		{
			for (int j = 0; j < img.cols; ++j)
			{
				y = static_cast<int>(floor(i - a*j*j)) + move;
				//cout << y << endl;
				A_refresh.at<uchar>(y, j) = img.at<uchar>(i, j);
				if (A_refresh.at<uchar>(y, j) == 255)
				{
					if (y < min_y)
					{
						min_y = y;
					}
					if (y > max_y)
					{
						max_y = y;
					}
				}
			}
		}

		//cout << min_y << " " << max_y << endl;

		Mat A_refresh_cut((max_y - min_y), img.cols, CV_8UC1, Scalar(0));

		for (int i = 0; i < A_refresh_cut.rows; ++i)
		{
			for (int j = 0; j < A_refresh_cut.cols; ++j)
			{
				A_refresh_cut.at<uchar>(i, j) = A_refresh.at<uchar>(i + min_y, j);
			}
		}

		int min_y_2;
		b_search_r(b_it_min, b_it_max, b_it_step, A_refresh_cut, max_a_b_pict, max_a_b_sum, max_a_b_a
			, max_a_b_b, max_a_b_i, min_a_b_y, min_a_b_y_2, a_b_move, a_it, min_y, min_y_2, move, max_move);
	}
	return 0;
}


int main(int argc, char** argv)
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	/*Reading image labels*/
	setlocale(LC_ALL, "rus");
	string fold_Path = "C:/Users/79855/Documents/Диплом/lg4000/";

	vector<string> names_ofFiles = getNamesOfFile(fold_Path);
	map<string, int> nums_names_ofFiles;
	for (int i = 0; i != names_ofFiles.size(); ++i) {
		nums_names_ofFiles.insert(pair<string, int>(names_ofFiles[i], i));
	}

	/*std::cout << "mymap contains:\n";
	for (auto it = nums_names_ofFiles.begin(); it != nums_names_ofFiles.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';*/
	cout << names_ofFiles.size() << endl;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	/*Reading images*/
	vector<Mat> Images;
	/*Раскоменть*/		for (int i = 0; i < names_ofFiles.size(); ++i)//100; ++i)

	{
		String cesta = fold_Path + names_ofFiles[i];
		Mat img = imread(cesta, IMREAD_GRAYSCALE);
		Images.push_back(img);
	}
	cout << "Картинки считались" << endl;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	/*Reading text info*/
	ifstream file("C:/Users/79855/Documents/Диплом/lg4000/params_mod_ext.txt");
	string row;
	vector<string> words;
	vector<int> numbs;
	pair<string, vector<int>> splited_pair;
	string name;
	vector<int> PupX;
	PupX.resize(names_ofFiles.size());
	vector<int> PupY;
	PupY.resize(names_ofFiles.size());
	vector<int> PupR;
	PupR.resize(names_ofFiles.size());
	vector<int> OPrUpA;
	OPrUpA.resize(names_ofFiles.size());
	vector<int> OPrUpX;
	OPrUpX.resize(names_ofFiles.size());
	vector<int> OPrUpY;
	OPrUpY.resize(names_ofFiles.size());
	vector<int> IriX;
	IriX.resize(names_ofFiles.size());
	vector<int> IriY;
	IriY.resize(names_ofFiles.size());
	vector<int> IriR;
	IriR.resize(names_ofFiles.size());

	if (!file)
	{
		cout << "Файл не открыт" << endl;
		return -1;
	}
	else
	{
		cout << "Файл открыт!" << endl;
		getline(file, row);
		while (!file.eof())
		{
			getline(file, row);
			splited_pair = spliter(row);
			name = splited_pair.first;
			numbs = splited_pair.second;
			if (numbs.size() > 1)
			{
				PupX[nums_names_ofFiles[name]] = numbs[0];
				PupY[nums_names_ofFiles[name]] = numbs[1];
				PupR[nums_names_ofFiles[name]] = numbs[2];
				IriX[nums_names_ofFiles[name]] = numbs[4];
				IriY[nums_names_ofFiles[name]] = numbs[5];
				IriR[nums_names_ofFiles[name]] = numbs[6];
				OPrUpA[nums_names_ofFiles[name]] = numbs[20];
				OPrUpX[nums_names_ofFiles[name]] = numbs[21];
				OPrUpY[nums_names_ofFiles[name]] = numbs[22];

			}
		}
	}
	/*for (int i = 0; i < names_ofFiles.size(); ++i)
	{
		cout << PupX[i] << " " << PupY[i] << endl;
	}*/
	file.close();
	cout << "Файл закрыт!" << endl;
	int pict = 2736;
	ofstream outfile("C:/Users/79855/Documents/Диплом/Answer/result3.txt");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	/*Main transformations*/

	for (int pict = names_ofFiles.size(); pict < names_ofFiles.size(); ++pict)//10; ++pict)
{
	//Mat edge;
	//Canny(img, edge, 50, 150, 3);
	//imwrite("edge.jpg", edge);
	cout << PupX[pict] << " " << PupY[pict] << " " << PupR[pict] << " " << OPrUpA[pict] << " " << OPrUpX[pict] << " " << OPrUpY[pict] << endl;
	Mat_<uchar> img = Images[pict];
	//imwrite("dif.jpg", img);

	int offset_X = static_cast<int>(floor(PupX[pict] * 0.05));
	//int offset_Y = static_cast<int>(floor(PupY[pict] * 0.15));

	int R_step = static_cast<int>(floor(IriR[pict]*1.5));
	int offset_Y = PupY[pict] + static_cast<int>(floor(PupY[pict] * 0.15)) - R_step;
	if (offset_Y < 0) { offset_Y = 0; }
	//Mat A_resize(PupY[pict], img.cols, CV_8UC1, Scalar(0));
	Mat A_resize(R_step, img.cols, CV_8UC1, Scalar(0));

	for (int i = 0; i < A_resize.rows; ++i)
	{
		for (int j = offset_X; j < A_resize.cols - offset_X; ++j)
		{
			A_resize.at<uchar>(i, j) = img.at<uchar>(i + offset_Y, j);;
		}
	}
	
	Mat vert(A_resize.rows, A_resize.cols, CV_8UC1, Scalar(0));
	Mat horiz(A_resize.rows, A_resize.cols, CV_8UC1, Scalar(0));
	int por = 5;//4
	grad_bin(A_resize, vert, horiz, por);
	Mat diff;
	diff = vert - horiz;

	Mat vert2(A_resize.rows, A_resize.cols, CV_8UC1, Scalar(0));
	Mat horiz2(A_resize.rows, A_resize.cols, CV_8UC1, Scalar(0));
	Mat diff2(A_resize.rows, A_resize.cols, CV_8UC1, Scalar(0));

	interpol(vert, vert2, "horiz");
	interpol(vert, horiz2, "vert");
	//interpol(diff, diff2, "horiz");

	diff2 = horiz2 - horiz;

	imwrite("dif_resized.jpg", A_resize);

	diff2.copyTo(A_resize);

	int max_a_b_sum = 0;
	int max_a_b_a = 0;
	int max_a_b_b = 0;
	int max_a_b_i = 0;
	int min_a_b_y = 0;
	int min_a_b_y_2 = 0;
	int a_b_move = 0;

	int a_it_min = 1500;
	int a_it_max = 2500;
	int a_it_step = 200;

	int b_it_min = 250;
	int b_it_max = 350;
	int b_it_step = 10;
	Mat max_a_b_pict;
	int max_move = 0;

	a_search_r(a_it_min, a_it_max, a_it_step, b_it_min, b_it_max, b_it_step
		, A_resize
		, max_a_b_pict, max_a_b_sum, max_a_b_a, max_a_b_b, max_a_b_i, min_a_b_y, min_a_b_y_2, a_b_move, max_move);
	
	/*a_search_r(int(max_a_b_a*0.9), int(max_a_b_a*1.1), int(max_a_b_a*0.01), b_it_min, b_it_max, b_it_step
		, A_resize, max_a_b_pict, max_a_b_sum, max_a_b_a, max_a_b_b, max_a_b_i, min_a_b_y, min_a_b_y_2, a_b_move);*/
	a_search_r(2097, 2100, 1, 300, 301, 1
		, A_resize, max_a_b_pict, max_a_b_sum, max_a_b_a, max_a_b_b, max_a_b_i, min_a_b_y, min_a_b_y_2, a_b_move, max_move);

	int max_a_b_c = offset_Y + max_move - static_cast<int>(floor(max_a_b_a*max_a_b_b*max_a_b_b / 1048576));
	
	//cout << "Pict:" << pict<<"// "<<max_a_b_a << " " << max_a_b_b << " " << max_a_b_c << " " << max_a_b_sum << endl;

	max_a_b_pict *= 0.5;

	for (int j = 0; j < max_a_b_pict.cols; ++j)
	{
		max_a_b_pict.at<uchar>(max_a_b_i, j) = 255;
	}

	string img_write_name = "C:/Users/79855/Documents/Диплом/Answer/result_line_";
	img_write_name += to_string(pict);
	img_write_name += ".jpg";

	imwrite(img_write_name, max_a_b_pict);


	Mat channels[3] = { img, img, img };
	Mat m;
	merge(channels, 3, m);
	
	double a = static_cast<double>(max_a_b_a) / 1048576;
	int x0 = max_a_b_b;
	int y0 = max_a_b_c;
	double a2 = static_cast<double>(OPrUpA[pict]) / 1048576;
	int x02 = OPrUpX[pict];
	int y02 = OPrUpY[pict];
	for (int i = 0; i < m.cols; ++i)
	{
		int j = static_cast<int>(floor(a2*(i - x02)*(i - x02) + y02));
		if ((j < m.rows - 2) && (j > -1))
		{
			m.at<Vec3b>(j, i)[0] = 0;
			m.at<Vec3b>(j, i)[1] = 0;
			m.at<Vec3b>(j, i)[2] = 255;
			m.at<Vec3b>(j + 1, i)[0] = 0;
			m.at<Vec3b>(j + 1, i)[1] = 0;
			m.at<Vec3b>(j + 1, i)[2] = 255;
			m.at<Vec3b>(j + 2, i)[0] = 0;
			m.at<Vec3b>(j + 2, i)[1] = 0;
			m.at<Vec3b>(j + 2, i)[2] = 255;
		}
		j = static_cast<int>(floor(a*(i - x0)*(i - x0) + y0));
		if ((j < m.rows - 2)&&(j > -1))
		{
			m.at<Vec3b>(j, i)[0] = 0;
			m.at<Vec3b>(j, i)[1] = 255;
			m.at<Vec3b>(j, i)[2] = 0;
			m.at<Vec3b>(j + 1, i)[0] = 0;
			m.at<Vec3b>(j + 1, i)[1] = 255;
			m.at<Vec3b>(j + 1, i)[2] = 0;
			m.at<Vec3b>(j + 2, i)[0] = 0;
			m.at<Vec3b>(j + 2, i)[1] = 255;
			m.at<Vec3b>(j + 2, i)[2] = 0;
		}
	}

	img_write_name = "C:/Users/79855/Documents/Диплом/Answer/source_picture_";
	img_write_name += to_string(pict);
	img_write_name += ".jpg";
	imwrite(img_write_name, m);

	int s = 0;
	int e1 = 0;
	int e2 = 0;
	for (int i = 0; i < m.cols; ++i)
	{
		int q = IriR[pict] * IriR[pict] - (i - IriX[pict])*(i - IriX[pict]);
		if (q >= 0)
		{
			int f = static_cast<int>(floor(sqrt(q)));
			for (int j = IriY[pict] - f; j < IriY[pict] + f; ++j)
			{
				s += 1;
				if ((j < static_cast<int>(floor(a*(i - x0)*(i - x0) + y0))) && (j > static_cast<int>(floor(a2 * (i - x02)*(i - x02) + y02))))
				{
					e1 += 1;
				}
				if ((j > static_cast<int>(floor(a*(i - x0)*(i - x0) + y0))) && (j < static_cast<int>(floor(a2 * (i - x02)*(i - x02) + y02))))
				{
					e2 += 1;
				}
			}
		}
	}
	cout << "Pict:" << pict << "// " << max_a_b_a << " " << max_a_b_b << " " << max_a_b_c << " " << max(e1,e2) <<" "<< s << " " << static_cast<double>(max(e1, e2))/s << endl;
	//imwrite(names_ofFiles[pict], m);
	outfile << names_ofFiles[pict] << "   " << max_a_b_a << " " << max_a_b_b << " " << max_a_b_c << "    " << max(e1, e2) << " " << s << " " << static_cast<double>(max(e1, e2)) / s << endl;

}
	outfile.close();

	ifstream resfile("C:/Users/79855/Documents/Диплом/lg4000/result.txt");

	vector<int> QE;
	QE.resize(names_ofFiles.size());
	vector<int> QS;
	QS.resize(names_ofFiles.size());

	if (!resfile)
	{
		cout << "Файл не открыт" << endl;
		return -1;
	}
	else
	{
		cout << "Файл открыт!" << endl;
		while (!resfile.eof())
		{
			getline(resfile, row);
			splited_pair = spliter_resf(row);
			name = splited_pair.first;
			numbs = splited_pair.second;
			//cout << name << " " << nums_names_ofFiles[name] << " " <<numbs[3] << " " << numbs[4] << endl;
			if (numbs.size() > 1)
			{
				QE[nums_names_ofFiles[name]] = numbs[3];
				QS[nums_names_ofFiles[name]] = numbs[4];
			}
		}
	}
	resfile.close();

	int64 SE = 0;
	int64 SS = 0;
	for (int i = 0; i < names_ofFiles.size(); ++i)
	{
		SE += QE[i];
		SS += QS[i];
		//cout << QE[i] << " " << QS[i] << endl;
	}
	cout << SE << " " << SS << endl;
	return 0;
}