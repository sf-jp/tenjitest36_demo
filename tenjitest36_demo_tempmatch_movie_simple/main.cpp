/* OpenCV ����̃e���v���[�g�E�}�b�`���O*/
/*�@comctl32.lib��vfw32.lib (vfw��SDK�ɂ���j���K�v*/
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp> //for negapo���]

#include <iostream>
#include <ctype.h>

//map�p
#include <string>
#include <list>  /* �����`�̍��W�����߂邽��*/

//bitmap�@�p



using namespace cv;
using namespace std;

#define TEMP_NUM 91 /* template �t�@�C�����@*/

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

}; /*max��corner2�_�ł悢*/


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
	// (1)�q�X�g�O�������ψꉻ����  
	cvEqualizeHist(src_img, src_img2);

	cvSet(hist_img, cvScalarAll(255), 0);
	// (2)�q�X�g�O�������v�Z���C�X�P�[�����O    
	cvCalcHist(&src_img, hist, 0, NULL);
	cvGetMinMaxHistValue(hist, 0, &max_value, 0, 0);
	cvScale(hist->bins, hist->bins, ((double)hist_img->height) / max_value, 0);
	bin_w = cvRound((double)ch_width / hist_size);
	// (3)�q�X�g�O������`��    
	for (j = 0; j < hist_size; j++)
		cvRectangle(hist_img,
		cvPoint(j * bin_w, hist_img->height),
		cvPoint((j + 1) * bin_w,
		hist_img->height - cvRound(cvGetReal1D(hist->bins, j))),
		cvScalarAll(0), -1, 8, 0);

	/* return hist_img; */
	return src_img2;  //�q�X�g�O�������ψꉻ����img��Ԃ��΂悢

}

#define GMAX 255
//void getBinaryImage(int nx, int ny, Bitmap bmp, int[, ] f)
void getBinaryImage(int nx, int ny, IplImage *img, uchar f[])
{//�Q�l���f�[�^���擾
	int i, j;
	//Color col;

	//Bitmap bmp1 = new Bitmap(pb1.Image);
	for (j = 0; j< ny; j++)
	{
		for (i = 0; i < nx; i++)
		{
			//col = bmp.GetPixel(i, j);
			//if (chkSubjectBlack.Checked == true)//�}�`����
			//{

			//if (col.R < GMAX / 2){
			if (img->imageData[img->widthStep * j + i * 3 + 2] < GMAX / 2){  //RGB  R
				f[i, j] = 1;
			}
			else{
				f[i, j] = 0;
			}
			//}
			//else//�}�`����
			//{
			//	if (col.R > GMAX / 2) f[i, j] = 1;
			//	else f[i, j] = 0;
			//}
		}
	}
}



//�c���T�u���[�`��
void Expansion(IplImage *img, int nx, int ny)
{
	int i, j, k, b;
	int a[8];

	//���摜
	//uchar f[320*320];

	//��f�l��z��f[i,j]�Ɋi�[  
	//getBinaryImage(nx, ny, bmp, f);
	//getBinaryImage(nx, ny, img, f);

	//	 E - ��f = �Z�x0
	//	 E - ��f = �Z�x255

	//�c��
	for (j = 0; j<ny; j++)
	{
		for (i = 0; i<nx; i++)
		{
			if (i == 0 || i == nx - 1 || j == 0 || j == ny - 1) b = 0;//�w�i�摜�Ƃ���
			else
			{
				//if (f[i, j] == 1){
				if (img->imageData[img->widthStep * j + i * 3] == 255){
					b = 1;
				}
				else//0-��f�̂Ƃ���������
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
					for (k = 0; k<8; k++) b += a[k];//�ǂꂩ1�ł�1-��f�ł����1�ȏ�
				}
			}
			if (b >= 1)//���͂ɂP�ł�1-��f�������1-��f�Ƃ���
			{
				//if (chkSubjectBlack.Checked == true)//�}�`����
				//���摜��i,j�s�N�Z����RGB(0,0,0)�ɂ���΂悢
				//bmp.SetPixel(i, j, CV_RGB(0, 0, 0));
				img->imageData[img->widthStep * j + i * 3] = 0;     // B 
				img->imageData[img->widthStep * j + i * 3 + 1] = 0;  // G
				img->imageData[img->widthStep * j + i * 3 + 2] = 0;   // R


				//else
				//bmp.SetPixel(i, j, Color.FromArgb(255, 255, 255));
			}
			else
			{
				//if (chkSubjectBlack.Checked == true)//�}�`����
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
* ����A�u���v�̂悤�ȓ_���̏��Ȃ��_�����A
*�@�u���v�u���v���@�ق��̓_���̈ꕔ���u���v��
*�@�F������󋵂����肦��B
*�@�u���v���A�_���̑����摜���ɔF��������悤�ɂ���B
*
*
*
*/


int main(int argc, char *argv[]){
	int key_input; //�L�[����
	CvCapture* camera = NULL; //�J�����E�f�o�C�X
	//double w = 320, h = 240;
	double w = 640, h = 480;
	//double w = 1980, h = 1080;
	IplImage *img = NULL; //�摜�f�[�^�i�[�p�\����
	IplImage *grey = NULL; //�����p�摜�f�[�^
	//(��荞�݉摜�̃O���[�E�X�P�[��)
	IplImage *temp = NULL; //�e���v���[�g�p�摜�i�[�\����
	IplImage *dst = NULL; //���֏��i�[�p�\����
	IplImage *dst_roi = NULL; //���֏��i�[�p�\����
	double max_interlinkage = 0; //���֒l�̍ő�l��������ϐ�
	double min_interlinkage = 0; //���֒l�̍ŏ��l��������ϐ�
	CvPoint max_point; //���֒l�̍ő�l�̍��W��������ϐ�
	CvPoint min_point; //���֒l�̍ŏ��l�̍��W��������ϐ�
	CvPoint corner_point; //�^�[�Q�b�g�̂�������̎l�����W
	int i;

	//dot���o�p
	double dot_max_interlinkage = 0; //���֒l�̍ő�l��������ϐ�
	double dot_min_interlinkage = 0; //���֒l�̍ŏ��l��������ϐ�
	CvPoint dot_max_point; //���֒l�̍ő�l�̍��W��������ϐ�
	CvPoint dot_min_point; //���֒l�̍ŏ��l�̍��W��������ϐ�

	//���e���v���[�g�̔ԍ��ƁA���o�ʒu��\���@�����`�̍��W��list�B
	std::list< Rec_vertex> rects;
	list < Rec_vertex > ::iterator it;

	//�����̃e���v���[�g�̔z��i�����������z��j
	IplImage * templates[TEMP_NUM] = { NULL, NULL, NULL, NULL, NULL };  /* ���A��,���A���A��*/
	IplImage * dot_img = NULL;   //�_�݂̂̃C���[�W

	//���ɏo���Ђ炪�ȃC���[�W
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

	//�e���v���[�g�t�@�C����
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

	//�u���v�̔ԍ��B
	int num_a = 4;  // �u���v�̔ԍ����T���Ă����B
	IplImage *neighbor_img = NULL;  //�u���v���P�_�}�b�`�����Ƃ��`�F�b�N����ߖT�G���A
	CvRect roi_nei;
	IplImage *dst_nei = NULL;  //�}�b�`���O����

	/*
	char * hirafile[TEMP_NUM]
	= { "../../images/hiragana/a.png",
	"../../images/hiragana/i.png",
	};
	*/

	//�����`�̐F
	CvScalar req_color[TEMP_NUM] =
	{ CV_RGB(255, 0, 0),     /* �� */
	CV_RGB(0, 0, 255)      /* �� */
	};

	//canny edge
	IplImage *canny = NULL; //canny�����摜�f�[�^
	Mat src_mat, dst_mat;  //negapo ���]�p
	IplImage canny_negapo;  //negapo ���]�p
	//�R���g���X�g�����phist
	IplImage *hist_img;


	//�J�����E�f�o�C�X�̐擪�|�C���^���擾
	// (1)�R�}���h�����ɂ���Ďw�肳�ꂽ�ԍ��̃J�����ɑ΂���L���v�`���\���̂��쐬����
	//camera = cvCaptureFromCAM(-1);
	//if (argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))    
	//		camera = cvCreateCameraCapture(argc == 2 ? argv[1][0] - '0' : 0);
	/* ���̐ݒ�́C���p����J�����Ɉˑ����� */
	camera = cvCreateCameraCapture(0);  /* 0�Œ�ɂ���*/



	//�J�����E�f�o�C�X��������Ȃ��Ƃ��̏���
	if (camera == NULL){
		printf("Cannot Open Camera Device!\n");
		return(0);
	}


	// (2)�L���v�`���T�C�Y��ݒ肷��D  
	cvSetCaptureProperty(camera, CV_CAP_PROP_FRAME_WIDTH, w);
	cvSetCaptureProperty(camera, CV_CAP_PROP_FRAME_HEIGHT, h);


	//�e���v���[�g�摜�̓ǂݍ���(�O���[�E�X�P�[��)
	//if (argc >= 2)    temp = cvLoadImage(argv[1], 0);

	for (i = 0; i < TEMP_NUM; i++){
		templates[i] = cvLoadImage(tempfile[i], 0);
		//hira_img[i] = imread(hirafile[i]);
	}
	//dot�摜�̓ǂݍ��݁i�O���[�E�X�P�[���j
	dot_img = cvLoadImage(dotfile, 0);

	//Window �̐���
	cvNamedWindow("Show", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("grey", CV_WINDOW_AUTOSIZE);

	//debug
	cvNamedWindow("temp", CV_WINDOW_AUTOSIZE);
	cvShowImage("temp", temp);

	cvNamedWindow("canny", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("roi", CV_WINDOW_AUTOSIZE);


	//�J�����E�f�o�C�X����摜���擾
	img = cvQueryFrame(camera);
	//�e���v���[�g�E�}�b�`���O�ɗp���鑊�֒l�f�[�^���i�[����摜�̗̈�m��
	//�O���[�E�X�P�[���摜�p�ɗ̈�m��
	grey = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	//dst = cvCreateImage(cvSize(img->width - temp->width + 1, img->height - temp->height + 1), IPL_DEPTH_32F, 1);

	//�F���t���O�𗧂Ă邽�߂̔z��(img->width�~img->height)����������B



	//�e���v���[�g�}�b�`���O�𕔕��摜�ōs������
	//�܂��́A�悱
	//�c���𐧌����邱�Ƃɂ��A�u���v�́A�c�����ɁA���̓_���̈ꕔ�Ƃ��ēǂ݊ԈႤ���Ƃ�
	//�Ȃ��Ȃ������A�������i��u���v�̈ꕔ�j�ɂ܂��ԈႤ�B
	CvRect roi;
	IplImage *roi_img;
	roi.x = 0;
	roi.y = 0;
	roi.width = grey->width;
	roi.height = grey->height / 4;
	roi_img = cvCreateImage(cvSize(roi.width, roi.height), grey->depth, grey->nChannels);



	//����L���v�`���ƕ\���̃��[�v�̊J�n
	for (;;){
		//�J�����E�f�o�C�X����摜���擾
		//*** ���������@�t���[�����𗎂Ƃ����ق����悢��������Ȃ��B1/10���炢 *****
		img = cvQueryFrame(camera);

		//�摜���擾�ł��Ȃ������Ƃ��̃G���[����
		if (img == NULL){
			printf("Cannot Open Video Data!\n");
			break;
		}
		//�O���[�E�X�P�[���ɕϊ����Ċi�[
		cvCvtColor(img, grey, CV_BGR2GRAY);

		//�F���t���O�𗧂Ă邽�߂̔z�������������B

		//grey��ROI�ɃR�s�[����B
		cvSetImageROI(grey, roi);
		cvCopy(grey, roi_img);
		cvResetImageROI(grey);
		cvShowImage("roi", roi_img);

		//�S�Ẵe���v���[�g�ɑ΂��ă}�b�`���O���s��
		for (i = 0; i < TEMP_NUM; i++){

			dst = cvCreateImage(cvSize(img->width - templates[i]->width + 1, img->height - templates[i]->height + 1), IPL_DEPTH_32F, 1);
			dst_roi = cvCreateImage(cvSize(roi_img->width - templates[i]->width + 1, roi_img->height - templates[i]->height + 1), IPL_DEPTH_32F, 1);

			//�e���v���[�g�E�}�b�`���O�̂��߂̑��֒l�̌v�Z
			//cvMatchTemplate(grey, temp, dst, CV_TM_CCOEFF_NORMED);

			//�S��ʂł̌���
			//cvMatchTemplate(grey, templates[i], dst, CV_TM_CCOEFF_NORMED);
			//ROI�ł̌���
			//���W�͋��ʂ��Ďg����BROI�̌Œ蕔�̂݌������邱�Ƃŉ�g�����������Ă݂��B
			cvMatchTemplate(roi_img, templates[i], dst_roi, CV_TM_CCOEFF_NORMED);

			//�e���v���[�g�E�}�b�`���O�ł����΂�}�b�`���Ă��镔��������
			//cvMinMaxLoc(dst, &min_interlinkage, &max_interlinkage,
			//	&min_point, &max_point, NULL);

			cvMinMaxLoc(dst_roi, &min_interlinkage, &max_interlinkage,
				&min_point, &max_point, NULL);

			//debug
			//printf("min=%lf,max=%lf\n", min_interlinkage, max_interlinkage);
			//printf("char=%s , i = %d , min=%lf,max=%lf max_p.x=%d , max_p.y=%d \n",
			//(char*)&kana[i],i, min_interlinkage, max_interlinkage,max_point.x,max_point.y);


			//�}�b�`���Ă���ꏊ�i���W�j���e���v���[�g���ɕۑ�
			corner_point = cvPoint(max_point.x + templates[i]->width, max_point.y + templates[i]->height);
			Rec_vertex  vex = Rec_vertex(i, max_point, corner_point);

			//���֒l0.75 �ȉ��Ȃ��Lost ��\�����Ď��ɍs��
			if (max_interlinkage > 0.75){
				//if (max_interlinkage > 0.80){

				//debug
				printf("char=%s , i = %d , min=%lf,max=%lf max_p.x=%d , max_p.y=%d \n",
					(char*)&kana[i], i, min_interlinkage, max_interlinkage, max_point.x, max_point.y);

				//�����A�}�b�`�����̂��u���v�̏ꍇ�A���̍��ׂ̗̈�ɁAdot.bmp��
				//�}�b�`���O���Ȃ������ׂ�B�}�b�`����ꍇ�A���̕������ԈႦ�ēǂ�ł���B
				if (i == num_a &&  max_point.x >= templates[i]->width){
					//���ׂ̗̈���}�b�`���O����B
					neighbor_img = cvCreateImage(cvSize(templates[i]->width, templates[i]->height), grey->depth, grey->nChannels);
					dst_nei = cvCreateImage(cvSize(templates[i]->width - dot_img->width + 1, templates[i]->height - dot_img->height + 1), IPL_DEPTH_32F, 1);
					roi_nei.x = max_point.x - templates[i]->width;  //��̂ɂȂ��Ă��܂����A�u���v�̏ꍇ���ׂ̃G���A
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
						//�ׂ̃G���A�ɁA�_�����v����B���@���̓_�����u���v�ƔF�����Ă��܂��Ă���B
						//���̔F���ӏ��́u���v�Ƃ��Ȃ�
						//
						break;
					}
				}

				//�g��t����
				cvRectangle(img, max_point, corner_point, req_color[i], 2);

				//�Ђ炪�ȕ\��
				//putText(cvarrToMat(img), kana[i], max_point, cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 0, 200), 2, CV_AA);
				putText(cvarrToMat(img), kana[i], CvPoint(max_point.x,corner_point.y+10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 200), 2, CV_AA);

				//���̗̈�ɁA�F���ς݃t���O�𗧂Ă�B

			}
			else{
				//printf("Lost\n");
			}


			//�v�Z�pdst��s�x�@Release
			cvReleaseImage(&dst);
			cvReleaseImage(&dst_roi);

		}
		//�e���v���[�g�ʂ�����@�\��
		cvShowImage("Show", img);


		//�}�b�`���O�ӏ����l�p�ŕ`��
		//�ۑ����Ă������}�b�`���O�ӏ���S�ĕ`��
		//cvRectangle(img, max_point, corner_point, CV_RGB(255, 0, 0), 2);
		//it = rects.begin();
		//for (it = rects.begin(); it!= rects.end();it++){
		//while (rects.size() != 0){

		//Rec_vertex &r = rects.front();
		//cvRectangle(img, r.src, r.corner, req_color[r.num], 2);
		//cvRectangle(img, max_point, corner_point, req_color[i], 2);
		//printf("Detection\n");

		//rects.pop_front();
		//iterator��i�߂�
		//++it;

		//debug �s�x�摜�\�����Ă݂�
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
		//cvCanny(img, canny, 0.0, 30.0);  /*�׃X�g�@�B�@*/
		//cvCanny(img, canny, 5.0, 30.0);  /*�@�x�X�g�A�@*/
		//cvCanny(img, canny, 10.0, 30.0);  /*�@�܂��̂���������Ȃ��_�ł̓x�X�g�@�������������֊s�`�b�N�ɂȂ��Ăق����@*/

		//hist�������̂�canny�ɂ��Ă݂�
		//cvCanny(hist_img, canny, 64, 128);

		//cvCanny(img, canny, 0.0, 50.0);  /*�܂��܂��n�j �A�@*/
		//cvCanny(img, canny, 10.0, 50.0);  /*0, 50���m�C�Y���̂�*/
		//cvCanny(img, canny, 0.0, 60.0);  /*0,50���G�b�W���؂��@worse*/
		cvCanny(img, canny, 64, 128);

		//�c��
		//Expansion(canny, canny->width/img->widthStep, canny->height);

		cvShowImage("canny", canny);


		//canny �l�K�|���]

		src_mat = cvarrToMat(canny);
		bitwise_not(src_mat, dst_mat);
		canny_negapo = dst_mat;

		//�k����A�c��
		//Contraction() //�ǉ��v

		//�c��
		Expansion(&canny_negapo, (&canny_negapo)->width / img->widthStep, (&canny_negapo)->height);

		cvNamedWindow("canny_negapo", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
		cvShowImage("canny_negapo", &canny_negapo);


		//debug
		cvShowImage("grey", grey);

		//�L�[����
		//key_input = cvWaitKey(2);
		key_input = cvWaitKey(10);
		//ESC �ŏI��
		if (key_input == 0x1b){
			printf("ESC KEY Quit\n");
			break;
		}
	}
	//�������J��
	cvReleaseCapture(&camera);
	cvDestroyWindow("Show");
	return(0);

}