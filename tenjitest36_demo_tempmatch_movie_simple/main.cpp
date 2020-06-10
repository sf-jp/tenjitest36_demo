/* OpenCV 動画のテンプレート・マッチング*/
/*　comctl32.libとvfw32.lib (vfwはSDKにある）が必要*/
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp> //for negapo反転

#include <iostream>
#include <ctype.h>

//map用
#include <string>
#include <list>  /* 長方形の座標をためるため*/

//bitmap　用



using namespace cv;
using namespace std;

#define TEMP_NUM 91 /* template ファイル数　*/

class Rec_vertex{
public:
	int  num;  /*template number*/
	CvPoint  src;
	CvPoint  corner;

	Rec_vertex(CvPoint s, CvPoint c){
		src = c; corner = c;
	};

	Rec_vertex(int n, CvPoint s, CvPoint c){
		num = n;
		src = s; corner = c;
	};

}; /*maxとcorner2点でよい*/


IplImage *  getHistImage(IplImage *src_img)
{
	IplImage *hist_img;
	CvHistogram *hist;
	int i, j, bin_w, ch_width = 260;
	int hist_size = 256;
	float range_0[] = { 0, 256 };
	float *ranges[] = { range_0 };
	IplImage *src_img2;
	float max_value = 0;

	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_img = cvCreateImage(cvSize(ch_width, 200), 8, 1);

	src_img2 = cvCreateImage(cvGetSize(src_img), 8, 1);
	// (1)ヒストグラムを均一化する  
	cvEqualizeHist(src_img, src_img2);

	cvSet(hist_img, cvScalarAll(255), 0);
	// (2)ヒストグラムを計算し，スケーリング    
	cvCalcHist(&src_img, hist, 0, NULL);
	cvGetMinMaxHistValue(hist, 0, &max_value, 0, 0);
	cvScale(hist->bins, hist->bins, ((double)hist_img->height) / max_value, 0);
	bin_w = cvRound((double)ch_width / hist_size);
	// (3)ヒストグラムを描画    
	for (j = 0; j < hist_size; j++)
		cvRectangle(hist_img,
		cvPoint(j * bin_w, hist_img->height),
		cvPoint((j + 1) * bin_w,
		hist_img->height - cvRound(cvGetReal1D(hist->bins, j))),
		cvScalarAll(0), -1, 8, 0);

	/* return hist_img; */
	return src_img2;  //ヒストグラムを均一化したimgを返せばよい

}

#define GMAX 255
//void getBinaryImage(int nx, int ny, Bitmap bmp, int[, ] f)
void getBinaryImage(int nx, int ny, IplImage *img, uchar f[])
{//２値化データを取得
	int i, j;
	//Color col;

	//Bitmap bmp1 = new Bitmap(pb1.Image);
	for (j = 0; j< ny; j++)
	{
		for (i = 0; i < nx; i++)
		{
			//col = bmp.GetPixel(i, j);
			//if (chkSubjectBlack.Checked == true)//図形が黒
			//{

			//if (col.R < GMAX / 2){
			if (img->imageData[img->widthStep * j + i * 3 + 2] < GMAX / 2){  //RGB  R
				f[i, j] = 1;
			}
			else{
				f[i, j] = 0;
			}
			//}
			//else//図形が白
			//{
			//	if (col.R > GMAX / 2) f[i, j] = 1;
			//	else f[i, j] = 0;
			//}
		}
	}
}



//膨張サブルーチン
void Expansion(IplImage *img, int nx, int ny)
{
	int i, j, k, b;
	int a[8];

	//原画像
	//uchar f[320*320];

	//画素値を配列f[i,j]に格納  
	//getBinaryImage(nx, ny, bmp, f);
	//getBinaryImage(nx, ny, img, f);

	//	窶・ - 画素 = 濃度0
	//	窶・ - 画素 = 濃度255

	//膨張
	for (j = 0; j<ny; j++)
	{
		for (i = 0; i<nx; i++)
		{
			if (i == 0 || i == nx - 1 || j == 0 || j == ny - 1) b = 0;//背景画像とする
			else
			{
				//if (f[i, j] == 1){
				if (img->imageData[img->widthStep * j + i * 3] == 255){
					b = 1;
				}
				else//0-画素のときだけ調査
				{
					/*
					a[0] = f[i + 1, j];
					a[1] = f[i + 1, j - 1];
					a[2] = f[i, j - 1];
					a[3] = f[i - 1, j - 1];
					a[4] = f[i - 1, j];
					a[5] = f[i - 1, j + 1];
					a[6] = f[i, j + 1];
					a[7] = f[i + 1, j + 1];
					*/
					a[0] = img->imageData[img->widthStep * j + (i + 1) * 3] / 255;
					a[1] = img->imageData[img->widthStep *(j - 1) + (i + 1) * 3] / 255;
					a[2] = img->imageData[img->widthStep *(j - 1) + i * 3] / 255;
					a[3] = img->imageData[img->widthStep *(j - 1) + (i - 1) * 3] / 255;
					a[4] = img->imageData[img->widthStep *j + (i - 1) * 3] / 255;
					a[5] = img->imageData[img->widthStep *(j + 1) + (i - 1) * 3] / 255;
					a[6] = img->imageData[img->widthStep *(j + 1) + i * 3] / 255;
					a[7] = img->imageData[img->widthStep *(j + 1) + (i + 1) * 3] / 255;
					b = 0;
					for (k = 0; k<8; k++) b += a[k];//どれか1つでも1-画素であれば1以上
				}
			}
			if (b >= 1)//周囲に１つでも1-画素があれば1-画素とする
			{
				//if (chkSubjectBlack.Checked == true)//図形が黒
				//現画像のi,jピクセルをRGB(0,0,0)にすればよい
				//bmp.SetPixel(i, j, CV_RGB(0, 0, 0));
				img->imageData[img->widthStep * j + i * 3] = 0;     // B 
				img->imageData[img->widthStep * j + i * 3 + 1] = 0;  // G
				img->imageData[img->widthStep * j + i * 3 + 2] = 0;   // R


				//else
				//bmp.SetPixel(i, j, Color.FromArgb(255, 255, 255));
			}
			else
			{
				//if (chkSubjectBlack.Checked == true)//図形が黒
				//bmp.SetPixel(i, j, Color.FromArgb(255, 255, 255));
				//bmp.SetPixel(i, j, CV_RGB(255, 255, 255));
				img->imageData[img->widthStep * j + i * 3] = 255;     // B 
				img->imageData[img->widthStep * j + i * 3 + 1] = 255;  // G
				img->imageData[img->widthStep * j + i * 3 + 2] = 255;   // R

				//else
				//	bmp.SetPixel(i, j, Color.FromArgb(0, 0, 0));
			}
		}//next i
	}//next j
}






/*
* 現状、「あ」のような点数の少ない点字が、
*　「い」「え」等　ほかの点字の一部を「あ」と
*　認識する状況がありえる。
*　「え」等、点数の多い画像を先に認識させるようにする。
*
*
*
*/


int main(int argc, char *argv[]){
	int key_input; //キー入力
	CvCapture* camera = NULL; //カメラ・デバイス
	//double w = 320, h = 240;
	double w = 640, h = 480;
	//double w = 1980, h = 1080;
	IplImage *img = NULL; //画像データ格納用構造体
	IplImage *grey = NULL; //処理用画像データ
	//(取り込み画像のグレー・スケール)
	IplImage *temp = NULL; //テンプレート用画像格納構造体
	IplImage *dst = NULL; //相関情報格納用構造体
	IplImage *dst_roi = NULL; //相関情報格納用構造体
	double max_interlinkage = 0; //相関値の最大値を代入する変数
	double min_interlinkage = 0; //相関値の最小値を代入する変数
	CvPoint max_point; //相関値の最大値の座標を代入する変数
	CvPoint min_point; //相関値の最小値の座標を代入する変数
	CvPoint corner_point; //ターゲットのもう一方の四隅座標
	int i;

	//dot検出用
	double dot_max_interlinkage = 0; //相関値の最大値を代入する変数
	double dot_min_interlinkage = 0; //相関値の最小値を代入する変数
	CvPoint dot_max_point; //相関値の最大値の座標を代入する変数
	CvPoint dot_min_point; //相関値の最小値の座標を代入する変数

	//元テンプレートの番号と、検出位置を表す　長方形の座標のlist。
	std::list< Rec_vertex> rects;
	list < Rec_vertex > ::iterator it;

	//複数のテンプレートの配列（あいうえお想定）
	IplImage * templates[TEMP_NUM] = { NULL, NULL, NULL, NULL, NULL };  /* あ、い,う、え、お*/
	IplImage * dot_img = NULL;   //点のみのイメージ

	//下に出すひらがなイメージ
	Mat hira_img[TEMP_NUM];


	string kana[TEMP_NUM] = {
		"E", "E", "E", "E",
		"O", "O", "O", "O",
		"I", "I", "I", "I",
		"U", "U", "U", "U",
		/*
		"A", "A", "A", "A",
		*/
		"KA", "KA", "KA", "KA", "KA", "KA", "KA", "KA", "KA", "KA", "KA", "KA", "KA", "KA", "KA",
		"KE", "KE", "KE", "KE", "KE", "KE", "KE", "KE", "KE", "KE", "KE", "KE", "KE", "KE", "KE",
		"KI", "KI", "KI", "KI", "KI", "KI", "KI", "KI", "KI", "KI", "KI", "KI", "KI", "KI", "KI",
		"KO", "KO", "KO", "KO", "KO", "KO", "KO", "KO", "KO", "KO", "KO", "KO", "KO", "KO", "KO",
		"KU", "KU", "KU", "KU", "KU", "KU", "KU", "KU", "KU", "KU", "KU", "KU", "KU", "KU", "KU",
	};

	//テンプレートファイル名
	char * tempfile[TEMP_NUM]
		= {
		"../../images/tenji_sample/tenji_e1.bmp",
		"../../images/tenji_sample/tenji_e2.bmp",
		"../../images/tenji_sample/tenji_e3.bmp",
		"../../images/tenji_sample/tenji_e4.bmp",
		"../../images/tenji_sample/tenji_o1.bmp",
		"../../images/tenji_sample/tenji_o2.bmp",
		"../../images/tenji_sample/tenji_o3.bmp",
		"../../images/tenji_sample/tenji_o4.bmp",
		"../../images/tenji_sample/tenji_i1.bmp",
		"../../images/tenji_sample/tenji_i2.bmp",
		"../../images/tenji_sample/tenji_i3.bmp",
		"../../images/tenji_sample/tenji_i4.bmp",
		"../../images/tenji_sample/tenji_u1.bmp",
		"../../images/tenji_sample/tenji_u2.bmp",
		"../../images/tenji_sample/tenji_u3.bmp",
		"../../images/tenji_sample/tenji_u4.bmp",
		/*
		"../../images/tenji_sample/tenji_a1.bmp",
		"../../images/tenji_sample/tenji_a2.bmp",
		"../../images/tenji_sample/tenji_a3.bmp",
		"../../images/tenji_sample/tenji_a4.bmp",
		*/
		"../../images/tenji_sample/tenji_ka1.bmp",
		"../../images/tenji_sample/tenji_ka10.bmp",
		"../../images/tenji_sample/tenji_ka11.bmp",
		"../../images/tenji_sample/tenji_ka12.bmp",
		"../../images/tenji_sample/tenji_ka13.bmp",
		"../../images/tenji_sample/tenji_ka14.bmp",
		"../../images/tenji_sample/tenji_ka15.bmp",
		"../../images/tenji_sample/tenji_ka2.bmp",
		"../../images/tenji_sample/tenji_ka3.bmp",
		"../../images/tenji_sample/tenji_ka4.bmp",
		"../../images/tenji_sample/tenji_ka5.bmp",
		"../../images/tenji_sample/tenji_ka6.bmp",
		"../../images/tenji_sample/tenji_ka7.bmp",
		"../../images/tenji_sample/tenji_ka8.bmp",
		"../../images/tenji_sample/tenji_ka9.bmp",
		"../../images/tenji_sample/tenji_ke1.bmp",
		"../../images/tenji_sample/tenji_ke10.bmp",
		"../../images/tenji_sample/tenji_ke11.bmp",
		"../../images/tenji_sample/tenji_ke12.bmp",
		"../../images/tenji_sample/tenji_ke13.bmp",
		"../../images/tenji_sample/tenji_ke14.bmp",
		"../../images/tenji_sample/tenji_ke15.bmp",
		"../../images/tenji_sample/tenji_ke2.bmp",
		"../../images/tenji_sample/tenji_ke3.bmp",
		"../../images/tenji_sample/tenji_ke4.bmp",
		"../../images/tenji_sample/tenji_ke5.bmp",
		"../../images/tenji_sample/tenji_ke6.bmp",
		"../../images/tenji_sample/tenji_ke7.bmp",
		"../../images/tenji_sample/tenji_ke8.bmp",
		"../../images/tenji_sample/tenji_ke9.bmp",
		"../../images/tenji_sample/tenji_ki1.bmp",
		"../../images/tenji_sample/tenji_ki10.bmp",
		"../../images/tenji_sample/tenji_ki11.bmp",
		"../../images/tenji_sample/tenji_ki12.bmp",
		"../../images/tenji_sample/tenji_ki13.bmp",
		"../../images/tenji_sample/tenji_ki14.bmp",
		"../../images/tenji_sample/tenji_ki15.bmp",
		"../../images/tenji_sample/tenji_ki2.bmp",
		"../../images/tenji_sample/tenji_ki3.bmp",
		"../../images/tenji_sample/tenji_ki4.bmp",
		"../../images/tenji_sample/tenji_ki5.bmp",
		"../../images/tenji_sample/tenji_ki6.bmp",
		"../../images/tenji_sample/tenji_ki7.bmp",
		"../../images/tenji_sample/tenji_ki8.bmp",
		"../../images/tenji_sample/tenji_ki9.bmp",
		"../../images/tenji_sample/tenji_ko1.bmp",
		"../../images/tenji_sample/tenji_ko10.bmp",
		"../../images/tenji_sample/tenji_ko11.bmp",
		"../../images/tenji_sample/tenji_ko12.bmp",
		"../../images/tenji_sample/tenji_ko13.bmp",
		"../../images/tenji_sample/tenji_ko14.bmp",
		"../../images/tenji_sample/tenji_ko15.bmp",
		"../../images/tenji_sample/tenji_ko2.bmp",
		"../../images/tenji_sample/tenji_ko3.bmp",
		"../../images/tenji_sample/tenji_ko4.bmp",
		"../../images/tenji_sample/tenji_ko5.bmp",
		"../../images/tenji_sample/tenji_ko6.bmp",
		"../../images/tenji_sample/tenji_ko7.bmp",
		"../../images/tenji_sample/tenji_ko8.bmp",
		"../../images/tenji_sample/tenji_ko9.bmp",
		"../../images/tenji_sample/tenji_ku1.bmp",
		"../../images/tenji_sample/tenji_ku10.bmp",
		"../../images/tenji_sample/tenji_ku11.bmp",
		"../../images/tenji_sample/tenji_ku12.bmp",
		"../../images/tenji_sample/tenji_ku13.bmp",
		"../../images/tenji_sample/tenji_ku14.bmp",
		"../../images/tenji_sample/tenji_ku15.bmp",
		"../../images/tenji_sample/tenji_ku2.bmp",
		"../../images/tenji_sample/tenji_ku3.bmp",
		"../../images/tenji_sample/tenji_ku4.bmp",
		"../../images/tenji_sample/tenji_ku5.bmp",
		"../../images/tenji_sample/tenji_ku6.bmp",
		"../../images/tenji_sample/tenji_ku7.bmp",
		"../../images/tenji_sample/tenji_ku8.bmp",
		"../../images/tenji_sample/tenji_ku9.bmp",



	};


	char * dotfile = "../../images/tenji_sample/dot.bmp";

	//「あ」の番号。
	int num_a = 4;  // 「あ」の番号を控えておく。
	IplImage *neighbor_img = NULL;  //「あ」等１点マッチしたときチェックする近傍エリア
	CvRect roi_nei;
	IplImage *dst_nei = NULL;  //マッチング結果

	/*
	char * hirafile[TEMP_NUM]
	= { "../../images/hiragana/a.png",
	"../../images/hiragana/i.png",
	};
	*/

	//長方形の色
	CvScalar req_color[TEMP_NUM] =
	{ CV_RGB(255, 0, 0),     /* 赤 */
	CV_RGB(0, 0, 255)      /* 青 */
	};

	//canny edge
	IplImage *canny = NULL; //canny処理画像データ
	Mat src_mat, dst_mat;  //negapo 反転用
	IplImage canny_negapo;  //negapo 反転用
	//コントラスト強調用hist
	IplImage *hist_img;


	//カメラ・デバイスの先頭ポインタを取得
	// (1)コマンド引数によって指定された番号のカメラに対するキャプチャ構造体を作成する
	//camera = cvCaptureFromCAM(-1);
	//if (argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))    
	//		camera = cvCreateCameraCapture(argc == 2 ? argv[1][0] - '0' : 0);
	/* この設定は，利用するカメラに依存する */
	camera = cvCreateCameraCapture(0);  /* 0固定にした*/



	//カメラ・デバイスが見つからないときの処理
	if (camera == NULL){
		printf("Cannot Open Camera Device!\n");
		return(0);
	}


	// (2)キャプチャサイズを設定する．  
	cvSetCaptureProperty(camera, CV_CAP_PROP_FRAME_WIDTH, w);
	cvSetCaptureProperty(camera, CV_CAP_PROP_FRAME_HEIGHT, h);


	//テンプレート画像の読み込み(グレー・スケール)
	//if (argc >= 2)    temp = cvLoadImage(argv[1], 0);

	for (i = 0; i < TEMP_NUM; i++){
		templates[i] = cvLoadImage(tempfile[i], 0);
		//hira_img[i] = imread(hirafile[i]);
	}
	//dot画像の読み込み（グレー・スケール）
	dot_img = cvLoadImage(dotfile, 0);

	//Window の生成
	cvNamedWindow("Show", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("grey", CV_WINDOW_AUTOSIZE);

	//debug
	cvNamedWindow("temp", CV_WINDOW_AUTOSIZE);
	cvShowImage("temp", temp);

	cvNamedWindow("canny", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("roi", CV_WINDOW_AUTOSIZE);


	//カメラ・デバイスから画像を取得
	img = cvQueryFrame(camera);
	//テンプレート・マッチングに用いる相関値データを格納する画像の領域確保
	//グレー・スケール画像用に領域確保
	grey = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	//dst = cvCreateImage(cvSize(img->width - temp->width + 1, img->height - temp->height + 1), IPL_DEPTH_32F, 1);

	//認識フラグを立てるための配列(img->width×img->height)を準備する。



	//テンプレートマッチングを部分画像で行う準備
	//まずは、よこ
	//縦幅を制限することにより、「あ」の、縦方向に、他の点字の一部として読み間違うことは
	//なくなったが、横方向（例「う」の一部）にまだ間違う。
	CvRect roi;
	IplImage *roi_img;
	roi.x = 0;
	roi.y = 0;
	roi.width = grey->width;
	roi.height = grey->height / 4;
	roi_img = cvCreateImage(cvSize(roi.width, roi.height), grey->depth, grey->nChannels);



	//動画キャプチャと表示のループの開始
	for (;;){
		//カメラ・デバイスから画像を取得
		//*** もう少し　フレーム数を落としたほうがよいかもしれない。1/10くらい *****
		img = cvQueryFrame(camera);

		//画像が取得できなかったときのエラー処理
		if (img == NULL){
			printf("Cannot Open Video Data!\n");
			break;
		}
		//グレー・スケールに変換して格納
		cvCvtColor(img, grey, CV_BGR2GRAY);

		//認識フラグを立てるための配列を初期化する。

		//greyをROIにコピーする。
		cvSetImageROI(grey, roi);
		cvCopy(grey, roi_img);
		cvResetImageROI(grey);
		cvShowImage("roi", roi_img);

		//全てのテンプレートに対してマッチングを行う
		for (i = 0; i < TEMP_NUM; i++){

			dst = cvCreateImage(cvSize(img->width - templates[i]->width + 1, img->height - templates[i]->height + 1), IPL_DEPTH_32F, 1);
			dst_roi = cvCreateImage(cvSize(roi_img->width - templates[i]->width + 1, roi_img->height - templates[i]->height + 1), IPL_DEPTH_32F, 1);

			//テンプレート・マッチングのための相関値の計算
			//cvMatchTemplate(grey, temp, dst, CV_TM_CCOEFF_NORMED);

			//全画面での検索
			//cvMatchTemplate(grey, templates[i], dst, CV_TM_CCOEFF_NORMED);
			//ROIでの検索
			//座標は共通して使える。ROIの固定部のみ検索することで画枠を小さくしてみた。
			cvMatchTemplate(roi_img, templates[i], dst_roi, CV_TM_CCOEFF_NORMED);

			//テンプレート・マッチングでいちばんマッチしている部分を検索
			//cvMinMaxLoc(dst, &min_interlinkage, &max_interlinkage,
			//	&min_point, &max_point, NULL);

			cvMinMaxLoc(dst_roi, &min_interlinkage, &max_interlinkage,
				&min_point, &max_point, NULL);

			//debug
			//printf("min=%lf,max=%lf\n", min_interlinkage, max_interlinkage);
			//printf("char=%s , i = %d , min=%lf,max=%lf max_p.x=%d , max_p.y=%d \n",
			//(char*)&kana[i],i, min_interlinkage, max_interlinkage,max_point.x,max_point.y);


			//マッチしている場所（座標）をテンプレート毎に保存
			corner_point = cvPoint(max_point.x + templates[i]->width, max_point.y + templates[i]->height);
			Rec_vertex  vex = Rec_vertex(i, max_point, corner_point);

			//相関値0.75 以下ならばLost を表示して次に行く
			if (max_interlinkage > 0.75){
				//if (max_interlinkage > 0.80){

				//debug
				printf("char=%s , i = %d , min=%lf,max=%lf max_p.x=%d , max_p.y=%d \n",
					(char*)&kana[i], i, min_interlinkage, max_interlinkage, max_point.x, max_point.y);

				//もし、マッチしたのが「あ」の場合、その左隣の領域に、dot.bmpが
				//マッチングしないか調べる。マッチする場合、他の文字を間違えて読んでいる。
				if (i == num_a &&  max_point.x >= templates[i]->width){
					//左隣の領域をマッチングする。
					neighbor_img = cvCreateImage(cvSize(templates[i]->width, templates[i]->height), grey->depth, grey->nChannels);
					dst_nei = cvCreateImage(cvSize(templates[i]->width - dot_img->width + 1, templates[i]->height - dot_img->height + 1), IPL_DEPTH_32F, 1);
					roi_nei.x = max_point.x - templates[i]->width;  //大体になってしまうが、「あ」の場合左隣のエリア
					roi_nei.y = max_point.y;
					roi_nei.width = templates[i]->width;
					roi_nei.height = templates[i]->height;

					cvSetImageROI(grey, roi_nei);
					cvCopy(grey, neighbor_img);
					cvResetImageROI(grey);

					cvMatchTemplate(neighbor_img, dot_img, dst_nei, CV_TM_CCOEFF_NORMED);

					cvMinMaxLoc(dst_nei, &dot_min_interlinkage, &dot_max_interlinkage,
						&dot_min_point, &dot_max_point, NULL);

					if (dot_max_interlinkage > 0.75){
						//隣のエリアに、点が合致する。→　他の点字を「あ」と認識してしまっている。
						//この認識箇所は「あ」としない
						//
						break;
					}
				}

				//枠を付ける
				cvRectangle(img, max_point, corner_point, req_color[i], 2);

				//ひらがな表示
				//putText(cvarrToMat(img), kana[i], max_point, cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 0, 200), 2, CV_AA);
				putText(cvarrToMat(img), kana[i], CvPoint(max_point.x,corner_point.y+10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 200), 2, CV_AA);

				//この領域に、認識済みフラグを立てる。

			}
			else{
				//printf("Lost\n");
			}


			//計算用dstを都度　Release
			cvReleaseImage(&dst);
			cvReleaseImage(&dst_roi);

		}
		//テンプレート通したら　表示
		cvShowImage("Show", img);


		//マッチング箇所を四角で描画
		//保存しておいたマッチング箇所を全て描画
		//cvRectangle(img, max_point, corner_point, CV_RGB(255, 0, 0), 2);
		//it = rects.begin();
		//for (it = rects.begin(); it!= rects.end();it++){
		//while (rects.size() != 0){

		//Rec_vertex &r = rects.front();
		//cvRectangle(img, r.src, r.corner, req_color[r.num], 2);
		//cvRectangle(img, max_point, corner_point, req_color[i], 2);
		//printf("Detection\n");

		//rects.pop_front();
		//iteratorを進める
		//++it;

		//debug 都度画像表示してみる
		//cvShowImage("Show", img);

		//}

		//cvShowImage("Show", img);

		//debug
		canny = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
		//canny = cvCreateImage(cvGetSize(hist_img), IPL_DEPTH_8U, 1);

		//canny
		//cvCanny(img, canny, 50.0, 200.0)
		//cvCanny(img, canny, 0.0, 128.0);
		//cvCanny(img, canny, 0.0, 0.0);
		//cvCanny(img, canny, 0.0, 30.0);  /*べスト　③　*/
		//cvCanny(img, canny, 5.0, 30.0);  /*　ベスト②　*/
		//cvCanny(img, canny, 10.0, 30.0);  /*　まわりのちらつきが少ない点ではベスト①ただもう少し輪郭チックになってほしい　*/

		//histしたものをcannyにしてみる
		//cvCanny(hist_img, canny, 64, 128);

		//cvCanny(img, canny, 0.0, 50.0);  /*まずまずＯＫ ②　*/
		//cvCanny(img, canny, 10.0, 50.0);  /*0, 50よりノイズがのる*/
		//cvCanny(img, canny, 0.0, 60.0);  /*0,50よりエッジが切れる　worse*/
		cvCanny(img, canny, 64, 128);

		//膨張
		//Expansion(canny, canny->width/img->widthStep, canny->height);

		cvShowImage("canny", canny);


		//canny ネガポ反転

		src_mat = cvarrToMat(canny);
		bitwise_not(src_mat, dst_mat);
		canny_negapo = dst_mat;

		//縮小後、膨張
		//Contraction() //追加要

		//膨張
		Expansion(&canny_negapo, (&canny_negapo)->width / img->widthStep, (&canny_negapo)->height);

		cvNamedWindow("canny_negapo", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
		cvShowImage("canny_negapo", &canny_negapo);


		//debug
		cvShowImage("grey", grey);

		//キー入力
		//key_input = cvWaitKey(2);
		key_input = cvWaitKey(10);
		//ESC で終了
		if (key_input == 0x1b){
			printf("ESC KEY Quit\n");
			break;
		}
	}
	//メモリ開放
	cvReleaseCapture(&camera);
	cvDestroyWindow("Show");
	return(0);

}