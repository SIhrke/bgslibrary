#include "LBMixtureOfGaussians.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 4

using namespace bgslibrary::algorithms;

LBMixtureOfGaussians::LBMixtureOfGaussians() :
  IBGS(quote(LBMixtureOfGaussians)),
  sensitivity(81), bgThreshold(83), 
  learningRate(59), noiseVariance(206)
{
  debug_construction(LBMixtureOfGaussians);
  initLoadSaveConfig(algorithmName);
}

LBMixtureOfGaussians::~LBMixtureOfGaussians() {
  debug_destruction(LBMixtureOfGaussians);
  delete m_pBGModel;
}

void LBMixtureOfGaussians::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage *frame = new IplImage(img_input);

  if (firstTime) {
    int w = frame->width;
    int h = frame->height;

    m_pBGModel = new lb::BGModelMog(w, h);
    m_pBGModel->InitModel(frame);
  }

  m_pBGModel->setBGModelParameter(0, sensitivity);
  m_pBGModel->setBGModelParameter(1, bgThreshold);
  m_pBGModel->setBGModelParameter(2, learningRate);
  m_pBGModel->setBGModelParameter(3, noiseVariance);

  m_pBGModel->UpdateModel(frame);

  img_foreground = cv::cvarrToMat(m_pBGModel->GetFG());
  img_background = cv::cvarrToMat(m_pBGModel->GetBG());

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;

  firstTime = false;
}

void LBMixtureOfGaussians::save_config(cv::FileStorage &fs) {
  fs << "sensitivity" << sensitivity;
  fs << "bgThreshold" << bgThreshold;
  fs << "learningRate" << learningRate;
  fs << "noiseVariance" << noiseVariance;
  fs << "showOutput" << showOutput;
}

void LBMixtureOfGaussians::load_config(cv::FileStorage &fs) {
  fs["sensitivity"] >> sensitivity;
  fs["bgThreshold"] >> bgThreshold;
  fs["learningRate"] >> learningRate;
  fs["noiseVariance"] >> noiseVariance;
  fs["showOutput"] >> showOutput;
}

#endif
