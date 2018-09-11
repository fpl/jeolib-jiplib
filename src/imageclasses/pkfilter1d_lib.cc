/**********************************************************************
pkfilter1d_lib.cc: program to filter raster images: median, min/max, morphological, filtering
Copyright (C) 2008-2016 Pieter Kempeneers

This file is part of pktools

pktools is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

pktools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pktools.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/
#include <assert.h>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <math.h>
#include <sys/types.h>
#include <stdio.h>
#include "base/Optionpk.h"
#include "fileclasses/FileReaderAscii.h"
#include "imageclasses/ImgRaster.h"
#include "algorithms/StatFactory.h"
#include "algorithms/Filter.h"
#include "apps/AppFactory.h"

using namespace std;
using namespace app;
using namespace filter;

/**
 * @param filter (type: std::string) filter function (nvalid, median, var, min, max, sum, mean, dilate, erode, close, open, mode (majority voting), only for classes), smoothnodata (smooth nodata values only) values, ismin, ismax, order (rank pixels in order), stdev, mrf, dwt, dwti, dwt_cut, dwt_cut_from, savgolay, percentile, proportion)
 * @param srf (type: std::string) list of ASCII files containing spectral response functions (two columns: wavelength response)
 * @param fwhm (type: double) list of full width half to apply spectral filtering (-fwhm band1 -fwhm band2 ...)
 * @param dz (type: int) (default: 3) filter kernel size in z (spectral/temporal dimension), must be odd (example: 3).
 * @param nodata (type: double) nodata value(s) (e.g., used for smoothnodata filter)
 * @param wavelet (type: std::string) (default: daubechies) wavelet type: daubechies,daubechies_centered, haar, haar_centered, bspline, bspline_centered
 * @param family (type: int) (default: 4) wavelet family (vanishing moment, see also http://www.gnu.org/software/gsl/manual/html_node/DWT-Initialization.html)
 * @param nl (type: int) (default: 2) Number of leftward (past) data points used in Savitzky-Golay filter)
 * @param nr (type: int) (default: 2) Number of rightward (future) data points used in Savitzky-Golay filter)
 * @param ld (type: int) (default: 0) order of the derivative desired in Savitzky-Golay filter (e.g., ld=0 for smoothed function)
 * @param m (type: int) (default: 2) order of the smoothing polynomial in Savitzky-Golay filter, also equal to the highest conserved moment; usual values are m = 2 or m = 4)
 * @param class (type: short) class value(s) to use for density, erosion, dilation, openening and closing, thresholding
 * @param threshold (type: double) (default: 0) threshold value(s) to use for threshold filter (one for each class), or threshold to cut for dwt_cut (use 0 to keep all) or dwt_cut_from, or sigma for shift
 * @param tapz (type: double) taps used for spectral filtering
 * @param pad (type: std::string) (default: symmetric) Padding method for filtering (how to handle edge effects). Choose between: symmetric, replicate, circular, zero (pad with 0).
 * @param wavelengthIn (type: double) list of wavelengths in input spectrum (-win band1 -win band2 ...)
 * @param wavelengthOut (type: double) list of wavelengths in output spectrum (-wout band1 -wout band2 ...)
 * @param down (type: short) (default: 1) down sampling factor. Use value 1 for no downsampling. Use value n>1 for downsampling (aggregation)
 * @param interp (type: std::string) (default: akima) type of interpolation for spectral filtering (see http://www.gnu.org/software/gsl/manual/html_node/Interpolation-Types.html)
 * @param otype (type: std::string) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image
 * @param ct (type: std::string) color table (file with 5 columns: id R G B ALFA (0: transparent, 255: solid). Use none to ommit color table
 * @return shared pointer to image object
**/

shared_ptr<ImgRaster> ImgRaster::filter1d(app::AppFactory& app){
  try{
    shared_ptr<ImgRaster> imgWriter=createImg();
    filter1d(*imgWriter, app);
    return(imgWriter);
  }
  catch(string helpString){
    cerr << helpString << endl;
    return(0);
  }
}

/**
 * @param filter (type: std::string) filter function (nvalid, median, var, min, max, sum, mean, dilate, erode, close, open, mode (majority voting), only for classes), smoothnodata (smooth nodata values only) values, ismin, ismax, order (rank pixels in order), stdev, mrf, dwt, dwti, dwt_cut, dwt_cut_from, savgolay, percentile, proportion)
 * @param srf (type: std::string) list of ASCII files containing spectral response functions (two columns: wavelength response)
 * @param fwhm (type: double) list of full width half to apply spectral filtering (-fwhm band1 -fwhm band2 ...)
 * @param dz (type: int) (default: 3) filter kernel size in z (spectral/temporal dimension), must be odd (example: 3).
 * @param nodata (type: double) nodata value(s) (e.g., used for smoothnodata filter)
 * @param wavelet (type: std::string) (default: daubechies) wavelet type: daubechies,daubechies_centered, haar, haar_centered, bspline, bspline_centered
 * @param family (type: int) (default: 4) wavelet family (vanishing moment, see also http://www.gnu.org/software/gsl/manual/html_node/DWT-Initialization.html)
 * @param nl (type: int) (default: 2) Number of leftward (past) data points used in Savitzky-Golay filter)
 * @param nr (type: int) (default: 2) Number of rightward (future) data points used in Savitzky-Golay filter)
 * @param ld (type: int) (default: 0) order of the derivative desired in Savitzky-Golay filter (e.g., ld=0 for smoothed function)
 * @param m (type: int) (default: 2) order of the smoothing polynomial in Savitzky-Golay filter, also equal to the highest conserved moment; usual values are m = 2 or m = 4)
 * @param class (type: short) class value(s) to use for density, erosion, dilation, openening and closing, thresholding
 * @param threshold (type: double) (default: 0) threshold value(s) to use for threshold filter (one for each class), or threshold to cut for dwt_cut (use 0 to keep all) or dwt_cut_from, or sigma for shift
 * @param tapz (type: double) taps used for spectral filtering
 * @param pad (type: std::string) (default: symmetric) Padding method for filtering (how to handle edge effects). Choose between: symmetric, replicate, circular, zero (pad with 0).
 * @param wavelengthIn (type: double) list of wavelengths in input spectrum (-win band1 -win band2 ...)
 * @param wavelengthOut (type: double) list of wavelengths in output spectrum (-wout band1 -wout band2 ...)
 * @param down (type: short) (default: 1) down sampling factor. Use value 1 for no downsampling. Use value n>1 for downsampling (aggregation)
 * @param interp (type: std::string) (default: akima) type of interpolation for spectral filtering (see http://www.gnu.org/software/gsl/manual/html_node/Interpolation-Types.html)
 * @param otype (type: std::string) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image
 * @param ct (type: std::string) color table (file with 5 columns: id R G B ALFA (0: transparent, 255: solid). Use none to ommit color table
 * @return CE_None if successful, CE_Failure if failed
**/
CPLErr ImgRaster::filter1d(ImgRaster& imgWriter, app::AppFactory& app){
  Optionpk<std::string> method_opt("f", "filter", "filter function (nvalid, median, var, min, max, sum, mean, dilate, erode, close, open, mode (majority voting), only for classes), smoothnodata (smooth nodata values only) values, ismin, ismax, order (rank pixels in order), stdev, mrf, dwt, dwti, dwt_cut, dwt_cut_from, savgolay, percentile, proportion)");
  Optionpk<int> dimZ_opt("dz", "dz", "filter kernel size in z (spectral/temporal dimension), must be odd (example: 3).",3);
  Optionpk<std::string> wavelet_type_opt("wt", "wavelet", "wavelet type: daubechies,daubechies_centered, haar, haar_centered, bspline, bspline_centered", "daubechies");
  Optionpk<int> family_opt("wf", "family", "wavelet family (vanishing moment, see also http://www.gnu.org/software/gsl/manual/html_node/DWT-Initialization.html)", 4);
  Optionpk<int> savgolay_nl_opt("nl", "nl", "Number of leftward (past) data points used in Savitzky-Golay filter)", 2);
  Optionpk<int> savgolay_nr_opt("nr", "nr", "Number of rightward (future) data points used in Savitzky-Golay filter)", 2);
  Optionpk<int> savgolay_ld_opt("ld", "ld", "order of the derivative desired in Savitzky-Golay filter (e.g., ld=0 for smoothed function)", 0);
  Optionpk<int> savgolay_m_opt("m", "m", "order of the smoothing polynomial in Savitzky-Golay filter, also equal to the highest conserved moment; usual values are m = 2 or m = 4)", 2);
  Optionpk<short> class_opt("class", "class", "class value(s) to use for density, erosion, dilation, openening and closing, thresholding");
  Optionpk<double> threshold_opt("t", "threshold", "threshold value(s) to use for threshold filter (one for each class), or threshold to cut for dwt_cut (use 0 to keep all) or dwt_cut_from, or sigma for shift", 0);
  Optionpk<double> nodata_opt("nodata", "nodata", "nodata value(s) (e.g., used for smoothnodata filter)");
  Optionpk<double> tapz_opt("tapz", "tapz", "taps used for spectral filtering");
  Optionpk<string> padding_opt("pad","pad", "Padding method for filtering (how to handle edge effects). Choose between: symmetric, replicate, circular, zero (pad with 0).", "symmetric");
  Optionpk<double> fwhm_opt("fwhm", "fwhm", "list of full width half to apply spectral filtering (-fwhm band1 -fwhm band2 ...)");
  Optionpk<std::string> srf_opt("srf", "srf", "list of ASCII files containing spectral response functions (two columns: wavelength response)");
  Optionpk<double> wavelengthIn_opt("win", "wavelengthIn", "list of wavelengths in input spectrum (-win band1 -win band2 ...)");
  Optionpk<double> wavelengthOut_opt("wout", "wavelengthOut", "list of wavelengths in output spectrum (-wout band1 -wout band2 ...)");
  Optionpk<std::string> interpolationType_opt("interp", "interp", "type of interpolation for spectral filtering (see http://www.gnu.org/software/gsl/manual/html_node/Interpolation-Types.html)","akima");
  Optionpk<std::string>  otype_opt("ot", "otype", "Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image");
  Optionpk<string>  colorTable_opt("ct", "ct", "color table (file with 5 columns: id R G B ALFA (0: transparent, 255: solid). Use none to ommit color table");
  Optionpk<short> down_opt("d", "down", "down sampling factor. Use value 1 for no downsampling. Use value n>1 for downsampling (aggregation)", 1);
  Optionpk<short> verbose_opt("v", "verbose", "verbose mode if > 0", 0,2);

  wavelet_type_opt.setHide(1);
  family_opt.setHide(1);
  savgolay_nl_opt.setHide(1);
  savgolay_nr_opt.setHide(1);
  savgolay_ld_opt.setHide(1);
  savgolay_m_opt.setHide(1);
  class_opt.setHide(1);
  threshold_opt.setHide(1);
  tapz_opt.setHide(1);
  padding_opt.setHide(1);
  wavelengthIn_opt.setHide(1);
  wavelengthOut_opt.setHide(1);
  down_opt.setHide(1);
  // eps_opt.setHide(1);
  // l1_opt.setHide(1);
  // l2_opt.setHide(1);
  // a1_opt.setHide(1);
  // a2_opt.setHide(1);
  interpolationType_opt.setHide(1);
  otype_opt.setHide(1);
  colorTable_opt.setHide(1);

  bool doProcess;//stop process when program was invoked with help option (-h --help)
  try{
    doProcess=method_opt.retrieveOption(app);
    // angle_opt.retrieveOption(app);
    srf_opt.retrieveOption(app);
    fwhm_opt.retrieveOption(app);
    dimZ_opt.retrieveOption(app);
    nodata_opt.retrieveOption(app);
    wavelet_type_opt.retrieveOption(app);
    family_opt.retrieveOption(app);
    savgolay_nl_opt.retrieveOption(app);
    savgolay_nr_opt.retrieveOption(app);
    savgolay_ld_opt.retrieveOption(app);
    savgolay_m_opt.retrieveOption(app);
    class_opt.retrieveOption(app);
    threshold_opt.retrieveOption(app);
    tapz_opt.retrieveOption(app);
    padding_opt.retrieveOption(app);
    wavelengthIn_opt.retrieveOption(app);
    wavelengthOut_opt.retrieveOption(app);
    down_opt.retrieveOption(app);
    // eps_opt.retrieveOption(app);
    // l1_opt.retrieveOption(app);
    // l2_opt.retrieveOption(app);
    // a1_opt.retrieveOption(app);
    // a2_opt.retrieveOption(app);
    interpolationType_opt.retrieveOption(app);
    otype_opt.retrieveOption(app);
    colorTable_opt.retrieveOption(app);
    verbose_opt.retrieveOption(app);
    if(!doProcess){
      cout << endl;
      std::ostringstream helpStream;
      helpStream << "short option -h shows basic options only, use long option --help to show all options" << std::endl;
      throw(helpStream.str());//help was invoked, stop processing
    }

    GDALDataType theType=GDT_Unknown;
    if(otype_opt.size()){
      theType=string2GDAL(otype_opt[0]);
      if(theType==GDT_Unknown)
        std::cout << "Warning: unknown output pixel type: " << otype_opt[0] << ", using input type as default" << std::endl;
    }

    if(theType==GDT_Unknown)
      theType=this->getGDALDataType();

    if(verbose_opt[0])
      std::cout << "Output pixel type:  " << GDALGetDataTypeName(theType) << endl;

    string errorString;
    unsigned int nband=this->nrOfBand();

    if(fwhm_opt.size())
      nband=fwhm_opt.size();
    else if(srf_opt.size())
      nband=srf_opt.size();
    else if(tapz_opt.size())
      nband=this->nrOfBand();
    else{
      if(method_opt.empty()){
        errorString="Error: no filter selected, use option -f";
        throw(errorString);
      }
      else if(verbose_opt[0])
        std::cout << "filter method: " << method_opt[0] << "=" << filter::Filter::getFilterType(method_opt[0]) << std::endl;
        // std::cout << "filter method: "<< filter::Filter::getFilterType(method_opt[0]) << std::endl;
      switch(filter::Filter::getFilterType(method_opt[0])){
      case(filter::dilate):
      case(filter::erode):
      case(filter::close):
      case(filter::open):
      case(filter::smooth):
        //implemented in spectral/temporal domain (dimZ>1) and spatial domain
        if(dimZ_opt.size())
          assert(dimZ_opt[0]>1);
      nband=this->nrOfBand();
      break;
      case(filter::dwt):
      case(filter::dwti):
      case(filter::dwt_cut):
      case(filter::smoothnodata):
        //implemented in spectral/temporal/spatial domain and nband always this->nrOfBand()
        nband=this->nrOfBand();
      break;
      case(filter::savgolay):
        nband=this->nrOfBand();
        if(dimZ_opt.empty())
          dimZ_opt.push_back(1);
      case(filter::dwt_cut_from):
        //only implemented in spectral/temporal domain
        if(dimZ_opt.size()){
          nband=this->nrOfBand();
          assert(threshold_opt.size());
        }
        break;
      //implemented in spectral/temporal/spatial domain and nband 1 if dimZ>0
      case(filter::sum):
      case(filter::mean):
      case(filter::min):
      case(filter::max):
      case(filter::var):
      case(filter::stdev):
      case(filter::nvalid):
      case(filter::median):
      case(filter::percentile):
      case(filter::proportion):
        //implemented in spectral/temporal/spatial domain and nband 1 if dimZ==1
        if(dimZ_opt.size()==1)
          if(dimZ_opt[0]==1)
            nband=1;
          else
            nband=this->nrOfBand();
      break;
      default:{
        cout << endl;
        std::ostringstream errorStream;
        errorStream << "filter method: " << method_opt[0] << "=" << filter::Filter::getFilterType(method_opt[0]) << " not implemented"<< std::endl;
        // errorStream << "filter " << method_opt[0] << " (" << )"<< " not implemented";
        throw(errorStream.str());
        break;
      }
      }
    }
    imgWriter.open((this->nrOfCol()+down_opt[0]-1)/down_opt[0],(this->nrOfRow()+down_opt[0]-1)/down_opt[0],nband,theType);
    imgWriter.setProjection(this->getProjection());
    double gt[6];
    this->getGeoTransform(gt);
    gt[1]*=down_opt[0];//dx
    gt[5]*=down_opt[0];//dy
    imgWriter.setGeoTransform(gt);

    if(colorTable_opt.size()){
      if(colorTable_opt[0]!="none"){
        if(verbose_opt[0])
          cout << "set colortable " << colorTable_opt[0] << endl;
        assert(imgWriter.getDataType()==GDT_Byte);
        imgWriter.setColorTable(colorTable_opt[0]);
      }
    }
    else if(this->getColorTable()!=NULL)
      imgWriter.setColorTable(this->getColorTable());

    if(nodata_opt.size()){
      for(unsigned int iband=0;iband<imgWriter.nrOfBand();++iband)
        imgWriter.GDALSetNoDataValue(nodata_opt[0],iband);
    }

    filter::Filter filter1d;
    if(verbose_opt[0])
      cout << "Set padding to " << padding_opt[0] << endl;
    filter1d.setPadding(padding_opt[0]);
    if(class_opt.size()){
      if(verbose_opt[0])
        std::cout<< "class values: ";
      for(int iclass=0;iclass<class_opt.size();++iclass){
        filter1d.pushClass(class_opt[iclass]);
        if(verbose_opt[0])
          std::cout<< class_opt[iclass] << " ";
      }
      if(verbose_opt[0])
        std::cout<< std::endl;
    }

    if(nodata_opt.size()){
      if(verbose_opt[0])
        std::cout<< "mask values: ";
      for(unsigned int imask=0;imask<nodata_opt.size();++imask){
        if(verbose_opt[0])
          std::cout<< nodata_opt[imask] << " ";
        filter1d.pushNoDataValue(nodata_opt[imask]);
      }
      if(verbose_opt[0])
        std::cout<< std::endl;
    }
    filter1d.setThresholds(threshold_opt);

    if(tapz_opt.size()){
      if(verbose_opt[0]){
        std::cout << "taps: ";
        for(int itap=0;itap<tapz_opt.size();++itap)
          std::cout<< tapz_opt[itap] << " ";
        std::cout<< std::endl;
      }
      filter1d.setTaps(tapz_opt);
      //todo:this->filter3D.filter(imgWriter);
      filter1d.filter(*this,imgWriter);
      // filter1d.filter(input,output);
    }
    else if(fwhm_opt.size()){
      if(verbose_opt[0])
        std::cout << "spectral filtering to " << fwhm_opt.size() << " bands with provided fwhm " << std::endl;
      assert(wavelengthOut_opt.size()==fwhm_opt.size());
      assert(wavelengthIn_opt.size());

      Vector2d<double> lineInput(this->nrOfBand(),this->nrOfCol());
      Vector2d<double> lineOutput(wavelengthOut_opt.size(),this->nrOfCol());
      const char* pszMessage;
      void* pProgressArg=NULL;
      GDALProgressFunc pfnProgress=GDALTermProgress;
      double progress=0;
      pfnProgress(progress,pszMessage,pProgressArg);
      for(unsigned int y=0;y<this->nrOfRow();++y){
        if((y+1+down_opt[0]/2)%down_opt[0])
          continue;
        for(unsigned int iband=0;iband<this->nrOfBand();++iband)
          this->readData(lineInput[iband],y,iband);
        filter1d.applyFwhm<double>(wavelengthIn_opt,lineInput,wavelengthOut_opt,fwhm_opt, interpolationType_opt[0], lineOutput, down_opt[0], verbose_opt[0]);
        for(unsigned int iband=0;iband<imgWriter.nrOfBand();++iband){
          imgWriter.writeData(lineOutput[iband],y/down_opt[0],iband);
        }
        progress=(1.0+y)/imgWriter.nrOfRow();
        pfnProgress(progress,pszMessage,pProgressArg);
      }
    }
    else if(srf_opt.size()){
      if(verbose_opt[0])
        std::cout << "spectral filtering to " << srf_opt.size() << " bands with provided SRF " << std::endl;
      assert(wavelengthIn_opt.size());
      vector< Vector2d<double> > srf(srf_opt.size());//[0] srf_nr, [1]: wavelength, [2]: response
      ifstream srfFile;
      for(int isrf=0;isrf<srf_opt.size();++isrf){
        srf[isrf].resize(2);
        srfFile.open(srf_opt[isrf].c_str());
        double v;
        //add 0 to make sure srf is 0 at boundaries after interpolation step
        srf[isrf][0].push_back(0);
        srf[isrf][1].push_back(0);
        srf[isrf][0].push_back(1);
        srf[isrf][1].push_back(0);
        while(srfFile >> v){
          srf[isrf][0].push_back(v);
          srfFile >> v;
          srf[isrf][1].push_back(v);
        }
        srfFile.close();
        //add 0 to make sure srf[isrf] is 0 at boundaries after interpolation step
        srf[isrf][0].push_back(srf[isrf][0].back()+1);
        srf[isrf][1].push_back(0);
        srf[isrf][0].push_back(srf[isrf][0].back()+1);
        srf[isrf][1].push_back(0);
        if(verbose_opt[0])
          cout << "srf file details: " << srf[isrf][0].size() << " wavelengths defined" << endl;
      }
      assert(imgWriter.nrOfBand()==srf.size());
      double centreWavelength=0;
      Vector2d<double> lineInput(this->nrOfBand(),this->nrOfCol());
      const char* pszMessage;
      void* pProgressArg=NULL;
      GDALProgressFunc pfnProgress=GDALTermProgress;
      double progress=0;
      pfnProgress(progress,pszMessage,pProgressArg);
      for(unsigned int y=0;y<this->nrOfRow();++y){
        if((y+1+down_opt[0]/2)%down_opt[0])
          continue;
        for(unsigned int iband=0;iband<this->nrOfBand();++iband)
          this->readData(lineInput[iband],y,iband);
        for(unsigned int isrf=0;isrf<srf.size();++isrf){
          vector<double> lineOutput(imgWriter.nrOfCol());
          double delta=1.0;
          bool normalize=true;
          centreWavelength=filter1d.applySrf<double>(wavelengthIn_opt,lineInput,srf[isrf], interpolationType_opt[0], lineOutput, delta, normalize);
          if(verbose_opt[0])
            std::cout << "centre wavelength srf " << isrf << ": " << centreWavelength << std::endl;
          imgWriter.writeData(lineOutput,y/down_opt[0],isrf);
        }
        progress=(1.0+y)/imgWriter.nrOfRow();
        pfnProgress(progress,pszMessage,pProgressArg);
      }

    }
    else{
      switch(filter::Filter::getFilterType(method_opt[0])){
      case(filter::dilate):
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for morphological operator" << std::endl;
          exit(1);
        }
        if(verbose_opt[0])
          std::cout<< "1-D filtering: dilate" << std::endl;
        filter1d.morphology(*this,imgWriter,"dilate",dimZ_opt[0],verbose_opt[0]);
        break;
      case(filter::erode):
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for morphological operator" << std::endl;
          exit(1);
        }
        if(verbose_opt[0])
          std::cout<< "1-D filtering: erode" << std::endl;
        filter1d.morphology(*this,imgWriter,"erode",dimZ_opt[0],verbose_opt[0]);
        break;
      case(filter::close):{//closing
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for morphological operator" << std::endl;
          exit(1);
        }
        filter1d.morphology(*this,imgWriter,"dilate",dimZ_opt[0]);
        filter1d.morphology(imgWriter,imgWriter,"erode",dimZ_opt[0]);
        break;
      }
      case(filter::open):{//opening
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for morphological operator" << std::endl;
          exit(1);
        }
        filter1d.morphology(*this,imgWriter,"erode",dimZ_opt[0],verbose_opt[0]);
        filter1d.morphology(imgWriter,imgWriter,"dilate",dimZ_opt[0]);
        break;
      }
      case(filter::smooth):{//Smoothing filter
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for this filter" << std::endl;
          exit(1);
        }
        if(verbose_opt[0])
          std::cout<< "1-D filtering: smooth" << std::endl;
        filter1d.smooth(*this,imgWriter,dimZ_opt[0]);
        break;
      }
      case(filter::smoothnodata):{//Smoothing filter
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for this filter" << std::endl;
          exit(1);
        }
        if(verbose_opt[0])
          std::cout<< "1-D filtering: smooth" << std::endl;
        filter1d.smoothNoData(*this,interpolationType_opt[0],imgWriter);
        break;
      }
      case(filter::dwt):
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for this filter" << std::endl;
          exit(1);
        }
        if(verbose_opt[0])
          std::cout<< "DWT in spectral domain" << std::endl;
        filter1d.dwtForward(*this, imgWriter, wavelet_type_opt[0], family_opt[0]);
        break;
      case(filter::dwti):
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for this filter" << std::endl;
          exit(1);
        }
        if(verbose_opt[0])
          std::cout<< "inverse DWT in spectral domain" << std::endl;
        filter1d.dwtInverse(*this, imgWriter, wavelet_type_opt[0], family_opt[0]);
        break;
      case(filter::dwt_cut):
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for this filter" << std::endl;
          exit(1);
        }
        if(verbose_opt[0])
          std::cout<< "DWT approximation in spectral domain" << std::endl;
        filter1d.dwtCut(*this, imgWriter, wavelet_type_opt[0], family_opt[0], threshold_opt[0]);
        break;
      case(filter::dwt_cut_from):
        if(down_opt[0]!=1){
          std::cerr << "Error: down option not supported for this filter" << std::endl;
          exit(1);
        }
        if(verbose_opt[0])
          std::cout<< "DWT approximation in spectral domain" << std::endl;
        filter1d.dwtCutFrom(*this,imgWriter, wavelet_type_opt[0], family_opt[0], static_cast<int>(threshold_opt[0]));
        break;
      case(filter::savgolay):{
        assert(savgolay_nl_opt.size());
        assert(savgolay_nr_opt.size());
        assert(savgolay_ld_opt.size());
        assert(savgolay_m_opt.size());
        if(verbose_opt[0])
          std::cout << "Calculating Savitzky-Golay coefficients: " << endl;
        filter1d.getSavGolayCoefficients(tapz_opt, this->nrOfBand(), savgolay_nl_opt[0], savgolay_nr_opt[0], savgolay_ld_opt[0], savgolay_m_opt[0]);
        if(verbose_opt[0]){
          std::cout << "taps (size is " << tapz_opt.size() << "): ";
          for(int itap=0;itap<tapz_opt.size();++itap)
            std::cout<< tapz_opt[itap] << " ";
          std::cout<< std::endl;
        }
        filter1d.setTaps(tapz_opt);
        filter1d.filter(*this,imgWriter);
        break;
      }
      case(filter::percentile)://deliberate fall through
      case(filter::threshold):{//deliberate fall through
        if(threshold_opt.empty()){
          std::string errorString="Error: thresholds not set";
          throw(errorString);
        }
        filter1d.setThresholds(threshold_opt);
      }
      default:{
        if(dimZ_opt[0]==1)
          filter1d.stat(*this,imgWriter,method_opt[0]);
        else{
          if(down_opt[0]!=1){
            std::string errorString="Error: down not implemented for this filter";
            throw(errorString);
          }
          filter1d.filter(*this,imgWriter,method_opt[0],dimZ_opt[0]);
          }
        break;
      }
      }
    }
    return(CE_None);
  }
  catch(string predefinedString){
    std::cout << predefinedString << std::endl;
    return(CE_Failure);
  }
}
