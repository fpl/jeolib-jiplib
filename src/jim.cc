/**********************************************************************
jim.cc: class to read raster files
History
2016/06/24 - Created by Pieter Kempeneers
Change log
***********************************************************************/
// #include "config.h"
#include "json/json.h"
#include "pktools/algorithms/Filter.h"
#include "pktools/imageclasses/pksml_lib.h"
#include "jim.h"
// #include "Python.h"

using namespace jiplib;

///constructors
Jim::Jim() : m_nplane(1) {};
///constructor opening an image in memory using an external data pointer
Jim::Jim(void* dataPointer, int ncol, int nrow, int nplane, const GDALDataType& dataType){
  open(dataPointer,ncol,nrow,nplane,dataType);
}
///constructor opening a multiband image in memory using an external data pointer
Jim::Jim(std::vector<void*> dataPointers, int ncol, int nrow, int nplane, const GDALDataType& dataType){
  open(dataPointers,ncol,nrow,nplane,dataType);
}
///constructor input image
Jim::Jim(IMAGE *mia) : m_nplane(1){
  setMIA(mia,0);
}
///constructor output image
Jim::Jim(const std::string& filename, bool readData, unsigned int memory) : m_nplane(1), ImgRaster(filename,memory){
  if(readData){
    for(int iband=0;iband<nrOfBand();++iband)
      readDataDS(iband,iband);
  }
}
///constructor output image
Jim::Jim(const std::string& filename, const Jim& imgSrc, unsigned int memory, const std::vector<std::string>& options) : m_nplane(1), ImgRaster(filename, imgSrc, memory, options){}
///constructor output image
Jim::Jim(Jim& imgSrc, bool copyData) : m_nplane(1), ImgRaster(imgSrc, copyData){};
///constructor output image
Jim::Jim(const std::string& filename, int ncol, int nrow, int nband, const GDALDataType& dataType, const std::string& imageType, unsigned int memory, const std::vector<std::string>& options) : m_nplane(1), ImgRaster(filename, ncol, nrow, nband, dataType, imageType, memory, options){};
///constructor output image
Jim::Jim(int ncol, int nrow, int nband, const GDALDataType& dataType) : m_nplane(1), ImgRaster(ncol, nrow, nband, dataType){};
//test
// Jim::Jim(app::AppFactory &theApp): m_nplane(1), ImgRaster(theApp){};
Jim::Jim(app::AppFactory &theApp){reset();open(theApp);};

///destructor
Jim::~Jim(void){
  if(m_mia.size()){
    for(int iband=0;iband<m_mia.size();++iband)
      if(m_mia[iband])
        delete(m_mia[iband]);
    m_mia.clear();
  }
  // ImgRaster::reset();
  close();
}

///Create new shared pointer to Jim object
/**
 * @param input (type: std::string) input filename
 * @param nodata (type: double) Nodata value to put in image if out of bounds.
 * @param band (type: int) Bands to open, index starts from 0
 * @param ulx (type: double) Upper left x value bounding box
 * @param uly (type: double) Upper left y value bounding box
 * @param lrx (type: double) Lower right x value bounding box
 * @param lry (type: double) Lower right y value bounding box
 * @param dx (type: double) Resolution in x
 * @param dy (type: double) Resolution in y
 * @param resample (type: std::string) (default: GRIORA_NearestNeighbour) resample: GRIORA_NearestNeighbour|GRIORA_Bilinear|GRIORA_Cubic|GRIORA_CubicSpline|GRIORA_Lanczos|GRIORA_Average|GRIORA_Average|GRIORA_Gauss (check http://www.gdal.org/gdal_8h.html#a640ada511cbddeefac67c548e009d5a)
 * @param extent (type: std::string) get boundary from extent from polygons in vector file
 * @param mem (type: unsigned long) (default: 0) Buffer size (in MB) to read image data blocks in memory
 * @param ncol (type: int) Number of columns
 * @param nrow (type: int) Number of rows
 * @param nband (type: int) (default: 1) Number of bands
 * @param otype (type: std::string) (default: Byte) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64})
 * @param seed (type: unsigned long) (default: 0) seed value for random generator
 * @param mean (type: double) (default: 0) Mean value for random generator
 * @param sigma (type: double) (default: 0) Sigma value for random generator
 * @param description (type: std::string) Set image description
 * @param a_srs (type: std::string) Assign the spatial reference for the output file, e.g., psg:3035 to use European projection and force to European grid
 * @return shared pointer to new Jim object
 **/
std::shared_ptr<Jim> Jim::createImg(app::AppFactory &theApp){
  std::shared_ptr<Jim> pJim=std::make_shared<Jim>(theApp);
  return(pJim);
}
/* ///Create new shared pointer to Jim object */
/* /\** */
/*  * @param input (type: std::string) input filename */
/*  * @param nodata (type: double) Nodata value to put in image if out of bounds. */
/*  * @param band (type: int) Bands to open, index starts from 0 */
/*  * @param ulx (type: double) Upper left x value bounding box */
/*  * @param uly (type: double) Upper left y value bounding box */
/*  * @param lrx (type: double) Lower right x value bounding box */
/*  * @param lry (type: double) Lower right y value bounding box */
/*  * @param dx (type: double) Resolution in x */
/*  * @param dy (type: double) Resolution in y */
/*  * @param resample (type: std::string) (default: GRIORA_NearestNeighbour) resample: GRIORA_NearestNeighbour|GRIORA_Bilinear|GRIORA_Cubic|GRIORA_CubicSpline|GRIORA_Lanczos|GRIORA_Average|GRIORA_Average|GRIORA_Gauss (check http://www.gdal.org/gdal_8h.html#a640ada511cbddeefac67c548e009d5a) */
/*  * @param extent (type: std::string) get boundary from extent from polygons in vector file */
/*  * @param mem (type: unsigned long) (default: 0) Buffer size (in MB) to read image data blocks in memory */
/*  * @param ncol (type: int) Number of columns */
/*  * @param nrow (type: int) Number of rows */
/*  * @param nband (type: int) (default: 1) Number of bands */
/*  * @param otype (type: std::string) (default: Byte) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}) */
/*  * @param seed (type: unsigned long) (default: 0) seed value for random generator */
/*  * @param mean (type: double) (default: 0) Mean value for random generator */
/*  * @param sigma (type: double) (default: 0) Sigma value for random generator */
/*  * @param description (type: std::string) Set image description */
/*  * @param a_srs (type: std::string) Assign the spatial reference for the output file, e.g., psg:3035 to use European projection and force to European grid */
/*  * @return shared pointer to new Jim object */
/*  **\/ */
std::shared_ptr<Jim> Jim::createImg() {
  return(std::make_shared<Jim>());
};

/* ///Create new shared pointer to Jim object */
/* /\** */
/*  * @param input (type: std::string) input filename */
/*  * @param nodata (type: double) Nodata value to put in image if out of bounds. */
/*  * @param band (type: int) Bands to open, index starts from 0 */
/*  * @param ulx (type: double) Upper left x value bounding box */
/*  * @param uly (type: double) Upper left y value bounding box */
/*  * @param lrx (type: double) Lower right x value bounding box */
/*  * @param lry (type: double) Lower right y value bounding box */
/*  * @param dx (type: double) Resolution in x */
/*  * @param dy (type: double) Resolution in y */
/*  * @param resample (type: std::string) (default: GRIORA_NearestNeighbour) resample: GRIORA_NearestNeighbour|GRIORA_Bilinear|GRIORA_Cubic|GRIORA_CubicSpline|GRIORA_Lanczos|GRIORA_Average|GRIORA_Average|GRIORA_Gauss (check http://www.gdal.org/gdal_8h.html#a640ada511cbddeefac67c548e009d5a) */
/*  * @param extent (type: std::string) get boundary from extent from polygons in vector file */
/*  * @param mem (type: unsigned long) (default: 0) Buffer size (in MB) to read image data blocks in memory */
/*  * @param ncol (type: int) Number of columns */
/*  * @param nrow (type: int) Number of rows */
/*  * @param nband (type: int) (default: 1) Number of bands */
/*  * @param otype (type: std::string) (default: Byte) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}) */
/*  * @param seed (type: unsigned long) (default: 0) seed value for random generator */
/*  * @param mean (type: double) (default: 0) Mean value for random generator */
/*  * @param sigma (type: double) (default: 0) Sigma value for random generator */
/*  * @param description (type: std::string) Set image description */
/*  * @param a_srs (type: std::string) Assign the spatial reference for the output file, e.g., psg:3035 to use European projection and force to European grid */
/*  * @return shared pointer to new Jim object */
/*  **\/ */
std::shared_ptr<Jim> Jim::createImg(const std::shared_ptr<Jim> pSrc, bool copyData){
  std::shared_ptr<Jim> pJim=std::make_shared<Jim>(*pSrc,copyData);
  return(pJim);
}

/* ///Create new shared pointer to Jim object */
/**
  * @param input (type: std::string) input filename
  * @return shared pointer to new Jim object
**/
std::shared_ptr<Jim> Jim::createImg(const std::string filename, bool readData, unsigned int memory){
  std::shared_ptr<Jim> pJim=std::make_shared<Jim>(filename,readData,memory);
  // std::shared_ptr<Jim> pJim=std::make_shared<Jim>(filename,memory);
  return(pJim);
}

///Open an image for writing, based on an existing image object
CPLErr Jim::open(void* dataPointer, int ncol, int nrow, int nplane, const GDALDataType& dataType){
  m_ncol=ncol;
  m_nrow=nrow;
  m_nplane=nplane;
  m_nband=1;
  m_dataType=dataType;
  m_data.resize(m_nband);
  m_begin.resize(m_nband);
  m_end.resize(m_nband);
  m_blockSize=nrow;//memory contains entire image and has been read already
  if(dataPointer){
    m_data[0]=(uint8_t*)dataPointer;
    m_begin[0]=0;
    m_end[0]=m_begin[0]+m_blockSize;
    /*
    for(int iband=0;iband<m_nband;++iband){
      m_data[iband]=(char *)dataPointer+iband*ncol*nrow*nplane*getDataTypeSizeBytes();
      m_begin[iband]=0;
      m_end[iband]=m_begin[iband]+m_blockSize;
    }
*/
    // m_externalData=true;
    return(CE_None);
  }
  else
    return(CE_Failure);
}

///Open a multiband image for writing, based on an existing image object
CPLErr Jim::open(std::vector<void*> dataPointers, int ncol, int nrow, int nplane, const GDALDataType& dataType){
  m_ncol=ncol;
  m_nrow=nrow;
  m_nplane=nplane;
  m_nband=dataPointers.size();
  m_dataType=dataType;
  m_data.resize(m_nband);
  m_begin.resize(m_nband);
  m_end.resize(m_nband);
  m_blockSize=nrow;//memory contains entire image and has been read already
  if(dataPointers.size()){
    for(int iband=0;iband<m_nband;++iband){
      if(dataPointers[iband]){
        m_data[iband]=(uint8_t*)dataPointers[iband]+iband*ncol*nrow*getDataTypeSizeBytes();
        m_begin[iband]=0;
        m_end[iband]=m_begin[iband]+m_blockSize;
      }
    }
    // m_externalData=true;
    return(CE_None);
  }
  else
    return(CE_Failure);
}

/**
 * @param filename Open a raster dataset with this filename
 * @param memory Available memory to cache image raster data (in MB)
 **/
CPLErr Jim::open(const std::string& filename, bool readData, unsigned int memory){
  reset();
  m_nplane=1;
  m_access=READ_ONLY;
  m_filename = filename;
  registerDriver();
  initMem(memory);
  for(int iband=0;iband<m_nband;++iband){
    m_begin[iband]=0;
    m_end[iband]=0;
  }
  if(readData){
    for(int iband=0;iband<nrOfBand();++iband)
      readDataDS(iband,iband);
  }
  return(CE_None);
}

///open dataset, read data and close (keep data in memory)
// CPLErr Jim::open(app::AppFactory &app) {
//   ImgRaster::open(app);
//   if(m_gds)
//     GDALClose(m_gds);
// }

///Open an image for writing, based on an existing image object
CPLErr Jim::open(Jim& imgSrc, bool copyData){
  m_ncol=imgSrc.nrOfCol();
  m_nrow=imgSrc.nrOfRow();
  m_nband=imgSrc.nrOfBand();
  m_dataType=imgSrc.getDataType();
  setProjection(imgSrc.getProjection());
  copyGeoTransform(imgSrc);
  imgSrc.getNoDataValues(m_noDataValues);
  imgSrc.getScale(m_scale);
  imgSrc.getOffset(m_offset);
  initMem(0);
  for(int iband=0;iband<m_nband;++iband){
    m_begin[iband]=0;
    m_end[iband]=m_begin[iband]+m_blockSize;
    if(copyData)
      imgSrc.copyData(m_data[iband],iband);
  }
  //todo: check if filename needs to be set, but as is it is used for writing, I don't think so.
  // if(imgSrc.getFileName()!=""){
  //   m_filename=imgSrc.getFileName();
  // std::cerr << "Warning: filename not set, dataset not defined yet" << std::endl;
  // }
  return(CE_None);
}

CPLErr Jim::open(app::AppFactory &app) {
  //input
  Optionpk<std::string> input_opt("fn", "filename", "filename");
  Optionpk<std::string> resample_opt("r", "resample", "resample: GRIORA_NearestNeighbour|GRIORA_Bilinear|GRIORA_Cubic|GRIORA_CubicSpline|GRIORA_Lanczos|GRIORA_Average|GRIORA_Average|GRIORA_Gauss (check http://www.gdal.org/gdal_8h.html#a640ada511cbddeefac67c548e009d5a)","GRIORA_NearestNeighbour");
  // Optionpk<std::string> extra_opt("extra", "extra", "RGDALRasterIOExtraArg (check http://www.gdal.org/structGDALRasterIOExtraArg.html)");
  // Optionpk<std::string> targetSRS_opt("t_srs", "t_srs", "Target spatial reference system in EPSG format (e.g., epsg:3035)");//todo
  //output
  Optionpk<double> nodata_opt("nodata", "nodata", "Nodata value to put in image.");
  Optionpk<int> nsample_opt("ns", "ncol", "Number of columns");
  Optionpk<int> nline_opt("nl", "nrow", "Number of rows");
  Optionpk<int> nband_opt("nb", "nband", "Number of bands",1);
  Optionpk<std::string> otype_opt("ot", "otype", "Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64})","Byte");
  Optionpk<unsigned long int> seed_opt("seed", "seed", "seed value for random generator",0);
  Optionpk<double> mean_opt("mean", "mean", "Mean value for random generator",0);
  Optionpk<double> stdev_opt("stdev", "stdev", "Standard deviation for Gaussian random generator",0);
  Optionpk<double> uniform_opt("uniform", "uniform", "start and end values for random value with uniform distribution",0);
  Optionpk<std::string> assignSRS_opt("a_srs", "a_srs", "Assign the spatial reference for the output file, e.g., psg:3035 to use European projection and force to European grid");
  // Optionpk<std::string> description_opt("d", "description", "Set image description");
  //input and output
  Optionpk<int> band_opt("b", "band", "Bands to open, index starts from 0");
  Optionpk<std::string> extent_opt("e", "extent", "get boundary from extent from polygons in vector file");
  Optionpk<double> ulx_opt("ulx", "ulx", "Upper left x value bounding box");
  Optionpk<double> uly_opt("uly", "uly", "Upper left y value bounding box");
  Optionpk<double> lrx_opt("lrx", "lrx", "Lower right x value bounding box");
  Optionpk<double> lry_opt("lry", "lry", "Lower right y value bounding box");
  Optionpk<double> dx_opt("dx", "dx", "Resolution in x");
  Optionpk<double> dy_opt("dy", "dy", "Resolution in y");
  Optionpk<std::string> access_opt("access", "access", "access (READ_ONLY, UPDATE)","READ_ONLY",2);
  Optionpk<bool> noread_opt("noread", "noread", "do not read data when opening",false);
  Optionpk<bool> band2plane_opt("band2plane", "band2plane", "read bands as planes",false);
  Optionpk<unsigned long int>  memory_opt("mem", "mem", "Buffer size (in MB) to read image data blocks in memory",0,1);

  bool doProcess;//stop process when program was invoked with help option (-h --help)
  try{
    doProcess=input_opt.retrieveOption(app);
    nodata_opt.retrieveOption(app);
    band_opt.retrieveOption(app);
    ulx_opt.retrieveOption(app);
    uly_opt.retrieveOption(app);
    lrx_opt.retrieveOption(app);
    lry_opt.retrieveOption(app);
    dx_opt.retrieveOption(app);
    dy_opt.retrieveOption(app);
    resample_opt.retrieveOption(app);
    extent_opt.retrieveOption(app);
    // extra_opt.retrieveOption(app);
    // targetSRS_opt.retrieveOption(app);
    nsample_opt.retrieveOption(app);
    nline_opt.retrieveOption(app);
    nband_opt.retrieveOption(app);
    otype_opt.retrieveOption(app);
    seed_opt.retrieveOption(app);
    mean_opt.retrieveOption(app);
    stdev_opt.retrieveOption(app);
    uniform_opt.retrieveOption(app);
    assignSRS_opt.retrieveOption(app);
    access_opt.retrieveOption(app);
    noread_opt.retrieveOption(app);
    band2plane_opt.retrieveOption(app);
    memory_opt.retrieveOption(app);
  }
  catch(std::string predefinedString){
    std::cout << predefinedString << std::endl;
  }
  if(!doProcess){
    std::cout << std::endl;
    std::ostringstream helpStream;
    helpStream << "exception thrown due to help info";
    throw(helpStream.str());//help was invoked, stop processing
  }

  std::vector<std::string> badKeys;
  app.badKeys(badKeys);
  if(badKeys.size()){
    std::ostringstream errorStream;
    if(badKeys.size()>1)
      errorStream << "Error: unknown keys: ";
    else
      errorStream << "Error: unknown key: ";
    for(int ikey=0;ikey<badKeys.size();++ikey){
      errorStream << badKeys[ikey] << " ";
    }
    errorStream << std::endl;
    throw(errorStream.str());
  }
  statfactory::StatFactory stat;

  //get bounding box from extentReader if defined
  VectorOgr extentReader;
  if(extent_opt.size()){
    double e_ulx;
    double e_uly;
    double e_lrx;
    double e_lry;
    for(int iextent=0;iextent<extent_opt.size();++iextent){
      extentReader.open(extent_opt[iextent]);
      extentReader.getExtent(e_ulx,e_uly,e_lrx,e_lry);
      ulx_opt.push_back(e_ulx);
      uly_opt.push_back(e_uly);
      lrx_opt.push_back(e_lrx);
      lry_opt.push_back(e_lry);
      extentReader.close();
    }
    e_ulx=stat.mymin(ulx_opt);
    e_uly=stat.mymax(uly_opt);
    e_lrx=stat.mymax(lrx_opt);
    e_lry=stat.mymin(lry_opt);
    ulx_opt.clear();
    uly_opt.clear();
    lrx_opt.clear();
    lrx_opt.clear();
    ulx_opt.push_back(e_ulx);
    uly_opt.push_back(e_uly);
    lrx_opt.push_back(e_lrx);
    lry_opt.push_back(e_lry);
  }

  if(input_opt.empty()){
    if(dx_opt.size()||dy_opt.size()){
      if(dx_opt.empty()){
        std::ostringstream errorStream;
        errorStream << "Warning: cell size in x not defined (use option --dx)." << std::endl;
        // ImgRaster();
        throw(errorStream.str());
      }
      if(dy_opt.empty()){
        std::ostringstream errorStream;
        errorStream << "Warning: cell size in y not defined (use option --dy)." << std::endl;
        // ImgRaster();
        throw(errorStream.str());
      }
      if(ulx_opt.empty()||uly_opt.empty()||lrx_opt.empty()||lry_opt.empty()){
        std::ostringstream errorStream;
        errorStream << "Warning: bounding box not defined (use options --ulx --uly --lrx --lry)." << std::endl;
        // ImgRaster();
        throw(errorStream.str());
      }
      nsample_opt.clear();
      nsample_opt.push_back((lrx_opt[0]-ulx_opt[0])/dx_opt[0]);
      nline_opt.clear();
      nline_opt.push_back((uly_opt[0]-lry_opt[0])/dy_opt[0]);
    }
    else if(nsample_opt.size()||nline_opt.size()){
      if(nsample_opt.empty()){
        std::ostringstream errorStream;
        errorStream << "Warning: no number of columns (use option --ncol)." << std::endl;
        // ImgRaster();
        throw(errorStream.str());
      }
      if(nline_opt.empty()){
        std::ostringstream errorStream;
        errorStream << "Warning: no number of rows (use option --nrow)." << std::endl;
        // ImgRaster();
        throw(errorStream.str());
      }
    }
    GDALDataType theType=string2GDAL(otype_opt[0]);
    // open(nsample_opt[0],nline_opt[0],nband_opt[0],theType);
    m_ncol = nsample_opt[0];
    m_nrow = nline_opt[0];
    m_nband = nband_opt[0];
    m_dataType = theType;
    initMem(0);
    for(int iband=0;iband<m_nband;++iband){
      m_begin[iband]=0;
      m_end[iband]=m_begin[iband]+m_blockSize;
    }
    if(m_filename!=""){
      // m_writeMode=true;
      m_access=WRITE;
      registerDriver();
    }
    if(ulx_opt.size()&&uly_opt.size()&&lrx_opt.size()&&lry_opt.size()){
      double gt[6];
      if(ulx_opt[0]<lrx_opt[0])
        gt[0]=ulx_opt[0];
      else
        gt[0]=0;
      if(dx_opt.size())
        gt[1]=dx_opt[0];
      else if(lrx_opt[0]-ulx_opt[0]>0){
        gt[1]=lrx_opt[0]-ulx_opt[0];
        gt[1]/=nrOfCol();
      }
      else
        gt[1]=1;
      gt[2]=0;
      if(uly_opt[0]>lry_opt[0])
        gt[3]=uly_opt[0];
      else
        gt[3]=0;
      gt[4]=0;
      if(dy_opt.size())
        gt[5]=-dy_opt[0];
      else if(uly_opt[0]-lry_opt[0]>0){
        gt[5]=lry_opt[0]-uly_opt[0];
        gt[5]/=nrOfRow();
      }
      else
        gt[5]=-1;
      setGeoTransform(gt);
      if(assignSRS_opt.size())
        setProjectionProj4(assignSRS_opt[0]);
    }
    gsl_rng* rndgen=stat.getRandomGenerator(seed_opt[0]);
    double value=mean_opt[0];
    std::vector<double> lineBuffer(nrOfCol(),value);
    double a=0;
    double b=1;
    std::string distribution="none";
    if(uniform_opt.size()>1){
      distribution="uniform";
      a=uniform_opt[0];
      b=uniform_opt[1];
    }
    else if(stdev_opt[0]>0){
      distribution="gaussian";
      a=mean_opt[0];
      b=stdev_opt[0];
    }
    else
      distribution="none";
    for(unsigned int iband=0;iband<nrOfBand();++iband){
      for(unsigned int irow=0;irow<nrOfRow();++irow){
        for(unsigned int icol=0;icol<nrOfCol();++icol){
          if(stat.getDistributionType(distribution)==statfactory::StatFactory::none)
            break;
          else
            value=stat.getRandomValue(rndgen,distribution,a,b);
          lineBuffer[icol]=value;
        }
        writeData(lineBuffer,irow,iband);
      }
    }
    stat.freeRandomGenerator(rndgen);
  }
  else if(input_opt.size()){
    setAccess(access_opt[0]);
    m_filename=input_opt[0];
    //set class member variables based on GDAL dataset
    registerDriver();
    //reset all class member variables according to options provided in app
    if(band_opt.empty()){
      while(band_opt.size()<nrOfBand())
        band_opt.push_back(band_opt.size());
    }
    m_nband=band_opt.size();
    if(ulx_opt.empty())
      ulx_opt.push_back(getUlx());
    if(uly_opt.empty())
      uly_opt.push_back(getUly());
    if(lrx_opt.empty())
      lrx_opt.push_back(getLrx());
    if(lry_opt.empty())
      lry_opt.push_back(getLry());
    if(dx_opt.empty())
      dx_opt.push_back(getDeltaX());
    if(dy_opt.empty())
      dy_opt.push_back(getDeltaY());

    //force bounding box to be within dataset
    if(ulx_opt[0]<getUlx())
      ulx_opt[0]=getUlx();
    if(uly_opt[0]>getUly())
      uly_opt[0]=getUly();
    if(lrx_opt[0]>getLrx())
      lrx_opt[0]=getLrx();
    if(lry_opt[0]<getLry())
      lry_opt[0]=getLry();

    //todo: reproject on the fly using
    // OGRSpatialReference::SetFromUserInput

    double gt[6];
    gt[0]=ulx_opt[0];
    gt[3]=uly_opt[0];
    gt[1]=dx_opt[0];//todo: adfGeotransform[1]: $cos(\alpha)\cdot\textrm{Xres}$
    gt[2]=0;//todo: $-sin(\alpha)\cdot\textrm{Xres}$
    gt[4]=0;//todo: $-sin(\alpha)\cdot\textrm{Yres}$
    gt[5]=-dy_opt[0];//todo: a$-cos(\alpha)\cdot\textrm{Yres}
    setGeoTransform(gt);

    if(assignSRS_opt.size())
      setProjectionProj4(assignSRS_opt[0]);
    m_resample=getGDALResample(resample_opt[0]);
    int nBufXSize=abs(static_cast<unsigned int>(ceil((lrx_opt[0]-ulx_opt[0])/dx_opt[0]-FLT_EPSILON)));
    int nBufYSize=abs(static_cast<unsigned int>(ceil((uly_opt[0]-lry_opt[0])/dy_opt[0]-FLT_EPSILON)));
    m_ncol=nBufXSize;
    m_nrow=nBufYSize;

    if(band2plane_opt[0]){
      m_nplane=m_nband;
      m_nband=1;
      initMem(memory_opt[0]);
      m_begin[0]=0;
      m_end[0]=m_begin[0]+m_blockSize;
      if(!noread_opt[0]){
        readDataPlanes(band_opt);
      }
    }
    else{
      if(!noread_opt[0]){
        //we initialize memory using class member variables instead of those read from GDAL dataset
        initMem(memory_opt[0]);
        for(int iband=0;iband<nrOfBand();++iband){
          m_begin[iband]=0;
          m_end[iband]=m_begin[iband]+m_blockSize;
          if(!noread_opt[0]){
            //we can not use readData(iband) because sequence of band_opt might not correspond bands in GDAL dataset
            readDataDS(iband,band_opt[iband]);
          }
        }
      }
      else{
        if(memory_opt[0]<=0)
          m_blockSize=nrOfRow();
        else{
          m_blockSize=static_cast<unsigned int>(memory_opt[0]*1000000/nrOfBand()/nrOfCol()/getDataTypeSizeBytes());
          if(getBlockSizeY(0))
            m_blockSize-=m_blockSize%getBlockSizeY(0);
          if(m_blockSize<1)
            m_blockSize=1;
          if(m_blockSize>nrOfRow())
            m_blockSize=nrOfRow();
        }
      }
    }
  }
  else{
    std::ostringstream errorStream;
    errorStream << "Warning: no number of rows or columns provided, nor input filename." << std::endl;
    // ImgRaster();
    throw(errorStream.str());
  }
  setNoData(nodata_opt);
  return(CE_None);
}

CPLErr Jim::readDataPlanes(std::vector<int> bands)
{
  CPLErr returnValue=CE_None;
  if(m_gds == NULL){
    std::string errorString="Error in readNewBlock";
    throw(errorString);
  }
  if(m_end[0]<m_blockSize)//first time
    m_end[0]=m_blockSize;
  if(m_end[0]>nrOfRow())
    m_end[0]=nrOfRow();

  int gds_ncol=m_gds->GetRasterXSize();
  int gds_nrow=m_gds->GetRasterYSize();
  int gds_nband=m_gds->GetRasterCount();
  double gds_gt[6];
  m_gds->GetGeoTransform(gds_gt);
  double gds_ulx=gds_gt[0];
  double gds_uly=gds_gt[3];
  double gds_lrx=gds_gt[0]+gds_ncol*gds_gt[1]+gds_nrow*gds_gt[2];
  double gds_lry=gds_gt[3]+gds_ncol*gds_gt[4]+gds_nrow*gds_gt[5];
  double gds_dx=gds_gt[1];
  double gds_dy=-gds_gt[5];
  double diffXm=getUlx()-gds_ulx;
  // double diffYm=gds_uly-getUly();

  // double dfXSize=diffXm/gds_dx;
  double dfXSize=(getLrx()-getUlx())/gds_dx;//x-size in pixels of region to read in original image
  double dfXOff=diffXm/gds_dx;
  // double dfYSize=diffYm/gds_dy;
  // double dfYSize=(getUly()-getLry())/gds_dy;//y-size in piyels of region to read in original image
  // double dfYOff=diffYm/gds_dy;
  // int nYOff=static_cast<int>(dfYOff);
  int nXSize=abs(static_cast<unsigned int>(ceil((getLrx()-getUlx())/gds_dx)));//x-size in pixels of region to read in original image
  int nXOff=static_cast<int>(dfXOff);
  if(nXSize>gds_ncol)
    nXSize=gds_ncol;

  double dfYSize=0;
  double dfYOff=0;
  int nYSize=0;
  int nYOff=0;

  GDALRasterIOExtraArg sExtraArg;
  INIT_RASTERIO_EXTRA_ARG(sExtraArg);
  sExtraArg.eResampleAlg = m_resample;
  dfYSize=(m_end[0]-m_begin[0])*getDeltaY()/gds_dy;//y-size in pixels of region to read in original image
  nYSize=abs(static_cast<unsigned int>(ceil((m_end[0]-m_begin[0])*getDeltaY()/gds_dy)));//y-size in pixels of region to read in original image
  if(nYSize>gds_nrow)
    nYSize=gds_nrow;
  dfYOff=(gds_uly-getUly())/gds_dy+m_begin[0]*getDeltaY()/gds_dy;
  nYOff=static_cast<int>(dfYOff);
  if(dfXOff-nXOff>0||dfYOff-nYOff>0||getDeltaX()<gds_dx||getDeltaX()>gds_dx||getDeltaY()<gds_dy||getDeltaY()>gds_dy){
    sExtraArg.bFloatingPointWindowValidity = TRUE;
    sExtraArg.dfXOff = dfXOff;
    sExtraArg.dfYOff = dfYOff;
    sExtraArg.dfXSize = dfXSize;
    sExtraArg.dfYSize = dfYSize;
  }
  else{
    sExtraArg.bFloatingPointWindowValidity = FALSE;
    sExtraArg.dfXOff = 0;
    sExtraArg.dfYOff = 0;
    sExtraArg.dfXSize = dfXSize;
    sExtraArg.dfYSize = dfYSize;
  }
  std::vector<int> gdalbands=bands;
  for(int iband=0;iband<bands.size();++iband)
    gdalbands[iband]=bands[iband]+1;

// eRWFlag	Either GF_Read to read a region of data, or GF_Write to write a region of data.
// nXOff	The pixel offset to the top left corner of the region of the band to be accessed. This would be zero to start from the left side.
// nYOff	The line offset to the top left corner of the region of the band to be accessed. This would be zero to start from the top.
// nXSize	The width of the region of the band to be accessed in pixels.
// nYSize	The height of the region of the band to be accessed in lines.
// pData	The buffer into which the data should be read, or from which it should be written. This buffer must contain at least nBufXSize * nBufYSize * nBandCount words of type eBufType. It is organized in left to right,top to bottom pixel order. Spacing is controlled by the nPixelSpace, and nLineSpace parameters.
// nBufXSize	the width of the buffer image into which the desired region is to be read, or from which it is to be written.
// nBufYSize	the height of the buffer image into which the desired region is to be read, or from which it is to be written.
// eBufType	the type of the pixel values in the pData data buffer. The pixel values will automatically be translated to/from the GDALRasterBand data type as needed.
// nBandCount	the number of bands being read or written.
// panBandMap	the list of nBandCount band numbers being read/written. Note band numbers are 1 based. This may be NULL to select the first nBandCount bands.
// nPixelSpace	The byte offset from the start of one pixel value in pData to the start of the next pixel value within a scanline. If defaulted (0) the size of the datatype eBufType is used.
// nLineSpace	The byte offset from the start of one scanline in pData to the start of the next. If defaulted (0) the size of the datatype eBufType * nBufXSize is used.
// nBandSpace	the byte offset from the start of one bands data to the start of the next. If defaulted (0) the value will be nLineSpace * nBufYSize implying band sequential organization of the data buffer.
// psExtraArg	(new in GDAL 2.0) pointer to a GDALRasterIOExtraArg structure with additional arguments to specify resampling and progress callback, or NULL for default behaviour. The GDAL_RASTERIO_RESAMPLING configuration option can also be defined to override the default resampling to one of BILINEAR, CUBIC, CUBICSPLINE, LANCZOS, AVERAGE or MODE.
  returnValue=getDataset()->RasterIO(GF_Read,nXOff,nYOff+m_begin[0],nXSize,nYSize,m_data[0],nrOfCol(),m_end[0]-m_begin[0],getGDALDataType(),gdalbands.size(),&gdalbands[0],0,0,0,&sExtraArg);
  return(returnValue);//new block was read
}

CPLErr Jim::initMem(unsigned int memory)
{
  if(memory<=0)
    m_blockSize=nrOfRow();
  else{
    m_blockSize=static_cast<unsigned int>(memory*1000000/nrOfBand()/nrOfCol()/getDataTypeSizeBytes());
    if(getBlockSizeY(0))
      m_blockSize-=m_blockSize%getBlockSizeY(0);
  }
  if(m_blockSize<1)
    m_blockSize=1;
  if(m_blockSize>nrOfRow())
    m_blockSize=nrOfRow();
  m_begin.resize(nrOfBand());
  m_end.resize(nrOfBand());
  freeMem();
  m_data.resize(nrOfBand());
  for(int iband=0;iband<nrOfBand();++iband){
    m_data[iband]=(void *) calloc(static_cast<size_t>(nrOfPlane()*nrOfCol()*m_blockSize),getDataTypeSizeBytes());
    if(!(m_data[iband])){
      std::string errorString="Error: could not allocate memory in initMem";
      throw(errorString);
    }
  }
  return(CE_None);
}

CPLErr Jim::close(){
  if(m_gds)
    GDALClose(m_gds);
  ImgRaster::reset();
}

///write to file previously set (eg., with setFile) without reset (keep data in memory)
CPLErr Jim::write(){
  //write, but do not reset
  if(m_data.size()&&m_filename.size()){
    for(int iband=0;iband<nrOfBand();++iband)
      writeNewBlock(nrOfRow(),iband);
  }
  char **papszOptions=NULL;
  for(std::vector<std::string>::const_iterator optionIt=m_options.begin();optionIt!=m_options.end();++optionIt)
    papszOptions=CSLAddString(papszOptions,optionIt->c_str());
  if(papszOptions)
    CSLDestroy(papszOptions);
  if(m_gds)
    GDALClose(m_gds);
  m_gds=0;
  // reset();
}

///write to file without reset (keep data in memory)
/**
 * @param filename Filename of the output raster dataset
 * @param oformat Image type. Currently only those formats where the drivers support the Create method can be written
 * @param co Creation option for output file. Multiple options can be specified.
 * @param nodata Nodata value to put in image.
 * @return CE_None if successful, else CE_Failure
 **/
CPLErr Jim::write(app::AppFactory &app){
  setFile(app);
  write();
}

///Create a JSON string from a Jim image
std::string Jim::jim2json(){
  Json::Value custom;
  custom["size"]=static_cast<int>(1);
  int iimg=0;
  Json::Value image;
  image["path"]=getFileName();
  std::string wktString=getProjectionRef();
  std::string key("EPSG");
  std::size_t foundEPSG=wktString.rfind(key);
  std::string fromEPSG=wktString.substr(foundEPSG);//EPSG","32633"]]'
  std::size_t foundFirstDigit=fromEPSG.find_first_of("0123456789");
  std::size_t foundLastDigit=fromEPSG.find_last_of("0123456789");
  std::string epsgString=fromEPSG.substr(foundFirstDigit,foundLastDigit-foundFirstDigit+1);
  image["epsg"]=atoi(epsgString.c_str());
  std::ostringstream os;
  os << iimg++;
  custom["0"]=image;
  Json::FastWriter fastWriter;
  return(fastWriter.write(custom));
}
///Create a custom collection from a Jim image
// std::string Jim::jim2custom(){
//   //todo:set directory as tmpnam will return path in /tmp
//   std::string custName = std::tmpnam(0);
//   std::vector<std::string> co;
//   co.push_back("COMPRESS=LZW");
//   setFile(custName,"GTiff",0,co);
//   write();
//   return(jim2json());
// }

///Clone as new shared pointer to ImgRaster object
/**
 * @param copyData (type: bool) value set to True if data needs to be copied
 * @return shared pointer to new ImgRaster object alllowing polymorphism
 */
std::shared_ptr<Jim> Jim::clone(bool copyData) {
  std::shared_ptr<Jim> pJim=std::dynamic_pointer_cast<Jim>(cloneImpl(copyData));
  if(pJim)
    return(pJim);
  else{
    std::cerr << "Warning: static pointer cast may slice object" << std::endl;
    return(std::static_pointer_cast<Jim>(cloneImpl(copyData)));
  }
}

///get size in Bytes of the current data type
/**
 * @return size in number of Bytes
 */
size_t Jim::getDataTypeSizeBytes(int band) const {
  switch (getDataType()){
  case JDT_UInt64:
  case JDT_Int64:
    return(static_cast<size_t>(8));
  default:
    return(ImgRaster::getDataTypeSizeBytes());
  }
}


/// convert single plane multiband image to single band image with multiple planes
CPLErr Jim::band2plane(){
  //temporary buffer
  m_data.resize(nrOfBand()+1);
  m_data[nrOfBand()]=(void *) calloc(static_cast<size_t>(nrOfCol()*m_blockSize),getDataTypeSizeBytes());
  //copy first band
  memcpy(m_data[nrOfBand()],m_data[0],getDataTypeSizeBytes()*nrOfCol()*m_blockSize);
  //delete temporary buffer
  free(m_data[nrOfBand()]);
  //erase m_data buffer
  m_data.erase(m_data.begin()+nrOfBand());
  //allocate memory
  m_data[0]=(void *) calloc(static_cast<size_t>(nrOfBand()*nrOfCol()*m_blockSize),getDataTypeSizeBytes());
  //copy rest of the bands
  for(size_t iband=1;iband<nrOfBand();++iband){
    //memcp
    memcpy(static_cast<char*>(m_data[0])+iband*nrOfCol()*nrOfRow(),static_cast<char*>(m_data[iband]),getDataTypeSizeBytes()*nrOfCol()*m_blockSize);
    // memcpy(m_data[0]+iband*nrOfCol()*nrOfRow(),m_data[iband],getDataTypeSizeBytes()*nrOfCol()*m_blockSize);
    free(m_data[iband]);
    m_data.erase(m_data.begin()+iband);
  }
  m_nplane=nrOfBand();
  m_nband=1;
}

/**
 *
 *
 * @param band the band to get the MIA image representation for
 *
 * @return pointer to MIA image representation
 */
IMAGE* Jim::getMIA(int band){
  if(getBlockSize()!=nrOfRow()){
    std::ostringstream s;
    s << "Error: increase memory to support MIA library functions (now at " << 100.0*getBlockSize()/nrOfRow() << "%)";
    throw(s.str());
  }
  if(m_mia.size()<band+1)
    m_mia.resize(band+1);
  if(m_mia[band])
    delete(m_mia[band]);
  m_mia[band]=new(IMAGE);
  m_mia[band]->p_im=m_data[band];/* Pointer to image data */
  m_mia[band]->DataType=getMIADataType();
  m_mia[band]->nx=nrOfCol();
  m_mia[band]->ny=nrOfRow();
  m_mia[band]->nz=nrOfPlane();
  m_mia[band]->NByte=m_mia[band]->nx * m_mia[band]->ny * m_mia[band]->nz * getDataTypeSizeBytes();//assumes image data type is not of bit type!!!
  //todo: remove m_mia[band]->vol and only rely on the getVolume function
  m_mia[band]->vol=0;//use getVolume() function
  m_mia[band]->lut=0;
  //USHORT *lut;   /* Pointer to colour map */
  //mia->g=getgetDataType();//not used
  return m_mia[band];
}

/**
 * set jim data pointer to the m_mia data pointer
 *
 * @param band the band for which the MIA image pointer needs to be set
 *
 * @return CE_None if successful
 */
CPLErr Jim::setMIA(int band){
  try{
    // if(m_mia->nz>1){
    //   std::string errorString="Error: MIA image with nz>1 not supported";
    //   throw(errorString);
    // }
    if(m_mia.size()<band+1){
      std::ostringstream s;
      s << "Error: illegal band number when setting MIA in Jim";
      throw(s.str());
    }
    if(m_ncol!=m_mia[band]->nx){
      std::ostringstream s;
      s << "Error: x dimension of image (" << m_ncol << ") does not match MIA (" << m_mia[band]->nx << ")";
      throw(s.str());
    }
    if(m_nrow!=m_mia[band]->ny){
      std::ostringstream s;
      s << "Error: y dimension of image (" << m_nrow << ") does not match MIA (" << m_mia[band]->ny << ")";
      throw(s.str());
    }
    if(m_nband<=band){
      std::ostringstream s;
      std::string errorString="Error: band exceeds number of bands in target image";
      throw(errorString);
    }
    // if(m_nband>1&&m_dataType!=MIA2GDALDataType(m_mia[band]->DataType)){
    if( (m_dataType!=MIA2JIPLIBDataType(m_mia[band]->DataType)) && nrOfBand() > 1){
      std::cout << "Warning: changing data type of multiband image, make sure to set all bands" << std::endl;
    }
    m_dataType=MIA2JIPLIBDataType(m_mia[band]->DataType);
    m_data[band]=(void *)m_mia[band]->p_im;
    // m_data[band]=(unsigned char *)m_mia[band]->p_im + band * nrOfRow() * nrOfCol() * (GDALGetDataTypeSize(getDataType())>>3);
    m_begin[band]=0;
    m_end[band]=m_begin[band]+getBlockSize();
  }
  catch(std::string errorString){
    std::cerr << errorString << std::endl;
    return(CE_Failure);
  }
  catch(...){
    return(CE_Failure);
  }
  return(CE_None);
}

/**
 * set Jim attributes from external MIA image
 *
 * @param mia the MIA image pointer to be set
 * @param band the band for which the MIA image pointer needs to be set
 *
 * @return C_None if successful
 */
CPLErr Jim::setMIA(IMAGE* mia, int band){
  try{
    if(nrOfBand()){
      if(m_ncol!=mia->nx){
        std::ostringstream s;
        s << "Error: x dimension of image (" << m_ncol << ") does not match MIA (" << mia->nx << ")";
        throw(s.str());
      }
      if(m_nrow!=mia->ny){
        std::ostringstream s;
        s << "Error: y dimension of image (" << m_nrow << ") does not match MIA (" << mia->ny << ")";
        throw(s.str());
      }
      if(m_nplane!=mia->nz){
        std::string errorString="Error: number of planes of images do not match";
        throw(errorString);
      }
      if(m_dataType!=MIA2JIPLIBDataType(m_mia[band]->DataType)){
        std::string errorString="Error: inconsistent data types for multiband image";
        throw(errorString);
      }
    }
    if(m_mia.size()<band+1){
      m_mia.resize(band+1);
      m_nband=band+1;
    }
    if(m_data.size()<band+1){
      m_data.resize(band+1);
      m_begin.resize(band+1);
      m_end.resize(band+1);
    }
    m_nplane=mia->nz;
    m_ncol=mia->nx;
    m_nrow=mia->ny;
    m_blockSize=m_nrow;
    m_mia[band]=mia;
    // setExternalData(true);//todo: need to fix memory leak when setMIA used for single band only! (either create vector<bool> m_externalData or only allow for setMIA all bands)
    this->setMIA(band);
  }
  catch(std::string errorString){
    std::cerr << errorString << std::endl;
    return(CE_Failure);
  }
  catch(...){
    return(CE_Failure);
  }
  return(CE_None);
}

///convert a GDAL data type to MIA data type
/**
 * @return MIA data type
 */
int Jim::getMIADataType(){
  switch (getDataType()){
  case GDT_Byte:
    return t_UCHAR;
  case GDT_UInt16:
    return t_USHORT;
  case GDT_Int16:
    return t_SHORT;
  case GDT_UInt32:
    return t_UINT32;
  case GDT_Int32:
    return t_INT32;
  case GDT_Float32:
    return t_FLOAT;
  case GDT_Float64:
    return t_DOUBLE;
  case JDT_UInt64:
    return t_UINT64;
  case JDT_Int64:
    return t_INT64;
  case t_UNSUPPORTED:
    return GDT_Unknown;
  default:
    return GDT_Unknown;
  }
}

///convert a JIPLIB to MIA data type
/**
 * @return MIA data type
 */
int Jim::JIPLIB2MIADataType(int aJIPLIBDataType){
  //function exists, but introduced for naming consistency
  if(aJIPLIBDataType==JDT_UInt64)
    return(t_UINT64);
  else if(aJIPLIBDataType==JDT_Int64)
    return(t_INT64);
  else
    return(GDAL2MIALDataType(aJIPLIBDataType));
}

///convert a GDAL to MIA data type
/**
 * @param aGDALDataType GDALDataType
 * @return MIA data type
 */
int Jim::GDAL2MIADataType(GDALDataType aGDALDataType){
  //function exists, but introduced for naming consistency
  return(GDAL2MIALDataType(aGDALDataType));
}

///convert a MIA data type to GDAL data type
/**
 * @param aMIADataType the MIA data type to be converted
 * @return GDAL data type (GDT_Byte, GDT_UInt16, GDT_Int16, GDT_UInt32, GDT_Int32, GDT_Float32, GDT_Float64)
 **/
int Jim::MIA2JIPLIBDataType(int aMIADataType){
  switch (aMIADataType){
  case t_UCHAR:
    return GDT_Byte;
  case t_USHORT:
    return GDT_UInt16;
  case t_SHORT:
    return GDT_Int16;
  case t_UINT32:
    return GDT_UInt32;
  case t_INT32:
    return GDT_Int32;
  case t_FLOAT:
    return GDT_Float32;
  case t_DOUBLE:
    return GDT_Float64;
  case t_UINT64:
    return JDT_UInt64;
  case t_INT64:
    return JDT_Int64;
  case t_UNSUPPORTED:
    return GDT_Unknown;
  default:
    return GDT_Unknown;
  }
}

///assignment operator
/**
 * @param refImg Use this as the reference image
 * @return true if image is equal to reference image
 **/
Jim& Jim::operator=(Jim& imgSrc)
{
  bool copyData=true;
  //check for assignment to self (of the form v=v)
  if(this==&imgSrc)
    return *this;
  else{
    ImgRaster::open(imgSrc,copyData);
    return *this;
  }
}


///relational == operator
/**
 * @param refImg Use this as the reference image
 * @return true if image is equal to reference image
 **/
bool Jim::isEqual(std::shared_ptr<Jim> refImg){
  bool isEqual=true;
  if(nrOfBand()!=refImg->nrOfBand())
    return(false);
  if(nrOfRow()!=refImg->nrOfRow())
    return(false);
  if(nrOfCol()!=refImg->nrOfCol())
    return(false);

  for(int iband=0;iband<nrOfBand();++iband){
    if(getDataType(iband)!=refImg->getDataType(iband)){
      isEqual=false;
      break;
    }
    IMAGE* refMIA=refImg->getMIA(iband);
    IMAGE* thisMIA=this->getMIA(iband);
    if(::imequalp(thisMIA,refMIA)){
      isEqual=false;
      break;
    }
  }
  return(isEqual);
}

///convert Jim image in memory returning Jim image (alias for crop)
std::shared_ptr<Jim> Jim::convert(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  ImgRaster::convert(*imgWriter, app);
  return(imgWriter);
}

///crop Jim image in memory returning Jim image
std::shared_ptr<Jim> Jim::crop(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  ImgRaster::crop(*imgWriter, app);
  return(imgWriter);
}

///crop Jim image in memory returning Jim image
std::shared_ptr<Jim> Jim::crop(VectorOgr& sampleReader, app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  ImgRaster::crop(sampleReader, *imgWriter, app);
  return(imgWriter);
}

///filter Jim image in spectral/temporal domain
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
std::shared_ptr<Jim> Jim::filter1d(app::AppFactory& theApp){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  /* std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(); */
  ImgRaster::filter1d(*imgWriter,theApp);
  return(imgWriter);
}

///filter Jim image in spatial domain
/**
 * @param filter (type: std::string) filter function (nvalid, median, var, min, max, sum, mean, dilate, erode, close, open, homog (central pixel must be identical to all other pixels within window), heterog (central pixel must be different than all other pixels within window), sobelx (horizontal edge detection), sobely (vertical edge detection), sobelxy (diagonal edge detection NE-SW),sobelyx (diagonal edge detection NW-SE), density, countid, mode (majority voting), only for classes), smooth, smoothnodata (smooth nodata values only) values, ismin, ismax, order (rank pixels in order), stdev, mrf, dwt, dwti, scramble, shift, percentile, proportion)
 * @param dx (type: double) (default: 3) filter kernel size in x, use odd values only
 * @param dy (type: double) (default: 3) filter kernel size in y, use odd values only
 * @param nodata (type: double) nodata value(s) (e.g., used for smoothnodata filter)
 * @param resampling-method (type: std::string) (default: near) Resampling method for shifting operation (near: nearest neighbour, bilinear: bi-linear interpolation).
 * @param wavelet (type: std::string) (default: daubechies) wavelet type: daubechies,daubechies_centered, haar, haar_centered, bspline, bspline_centered
 * @param family (type: int) (default: 4) wavelet family (vanishing moment, see also http://www.gnu.org/software/gsl/manual/html_node/DWT-Initialization.html)
 * @param class (type: short) class value(s) to use for density, erosion, dilation, openening and closing, thresholding
 * @param threshold (type: double) (default: 0) threshold value(s) to use for threshold filter (one for each class), or threshold to cut for dwt_cut (use 0 to keep all) or dwt_cut_from, or sigma for shift
 * @param tap (type: std::string) text file containing taps used for spatial filtering (from ul to lr). Use dimX and dimY to specify tap dimensions in x and y. Leave empty for not using taps
 * @param pad (type: std::string) (default: symmetric) Padding method for filtering (how to handle edge effects). Choose between: symmetric, replicate, circular, zero (pad with 0).
 * @param down (type: short) (default: 1) down sampling factor. Use value 1 for no downsampling. Use value n>1 for downsampling (aggregation)
 * @param beta (type: std::string) ASCII file with beta for each class transition in Markov Random Field
 * @param otype (type: std::string) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image
 * @param ct (type: std::string) color table (file with 5 columns: id R G B ALFA (0: transparent, 255: solid). Use none to ommit color table
 * @param circular (type: bool) (default: 0) circular disc kernel for dilation and erosion
 * @return shared pointer to image object
 **/
std::shared_ptr<Jim> Jim::filter2d(const app::AppFactory& theApp){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  // std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
  ImgRaster::filter2d(*imgWriter,theApp);
  return(imgWriter);
}

///get statistics on image
/**
 * @param scale (type: double) output=scale*input+offset
 * @param offset (type: double) output=scale*input+offset
 * @param mem (type: unsigned long) (default: 0) Buffer size (in MB) to read image data blocks in memory
 * @param function (type: std::string) (default: basic) Statistics function (invalid, valid, filename, basic, gdal, mean, median, var, skewness, kurtosis,stdev, sum, minmax, min, max, histogram, histogram2d, rmse, regression, regressionError, regressionPerpendicular
 * @param band (type: unsigned short) (default: 0) band(s) on which to calculate statistics
 * @param nodata (type: double) Set nodata value(s)
 * @param nbin (type: short) number of bins to calculate histogram
 * @param relative (type: bool) (default: 0) use percentiles for histogram to calculate histogram
 * @param ulx (type: double) Upper left x value bounding box
 * @param uly (type: double) Upper left y value bounding box
 * @param lrx (type: double) Lower right x value bounding box
 * @param lry (type: double) Lower right y value bounding box
 * @param down (type: short) (default: 1) Down sampling factor (for raster sample datasets only). Can be used to create grid points
 * @param rnd (type: unsigned int) (default: 0) generate random numbers
 * @param scale (type: double) Scale(s) for reading input image(s)
 * @param offset (type: double) Offset(s) for reading input image(s)
 * @param src_min (type: double) start reading source from this minimum value
 * @param src_max (type: double) stop reading source from this maximum value
 * @param kde (type: bool) (default: 0) Use Kernel density estimation when producing histogram. The standard deviation is estimated based on Silverman's rule of thumb
 * @return this object
 **/
std::multimap<std::string,std::string> Jim::getStats(app::AppFactory& theApp){return(ImgRaster::getStats(theApp));};

///create statistical profile
/**
 * @param function (type: std::string) Statistics function (mean, median, var, stdev, min, max, sum, mode (provide classes), ismin, ismax, proportion (provide classes), percentile, nvalid
 * @param perc (type: double) Percentile value(s) used for rule percentile
 * @param nodata (type: double) nodata value(s)
 * @param otype (type: std::string) (default: GDT_Unknown) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image
 * @return shared pointer to image object
 **/
std::shared_ptr<Jim> Jim::statProfile(app::AppFactory& theApp){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  // ImgRaster::statProfile(*imgWriter,theApp);
  // ImgRaster::statProfile(*imgWriter,theApp);
  ImgRaster::statProfile(*imgWriter,theApp);
  return(imgWriter);
}

///check the difference between two images
/**
 * @param reference (type: std::string) Reference vector dataset
 * @param ln (type: std::string) Layer name(s) in sample. Leave empty to select all (for vector reference datasets only)
 * @param band (type: unsigned int) (default: 0) Input (reference) raster band. Optionally, you can define different bands for input and reference bands respectively: -b 1 -b 0.
 * @param confusion (type: bool) (default: 1) Create confusion matrix (to std out)
 * @param lref (type: std::string) (default: label) Attribute name of the reference label (for vector reference datasets only)
 * @param class (type: std::string) List of class names.
 * @param reclass (type: short) List of class values (use same order as in classname option).
 * @param nodata (type: double) No data value(s) in input or reference dataset are ignored
 * @param mask (type: std::string) Use the first band of the specified file as a validity mask. Nodata values can be set with the option msknodata.
 * @param msknodata (type: double) (default: 0) Mask value(s) where image is invalid. Use negative value for valid data (example: use -t -1: if only -1 is valid value)
 * @param output (type: std::string) Output dataset (optional)
 * @param f (type: std::string) (default: SQLite) OGR format for output vector
 * @param lclass (type: std::string) (default: class) Attribute name of the classified label
 * @param cmf (type: std::string) (default: ascii) Format for confusion matrix (ascii or latex)
 * @param cmo (type: std::string) Output file for confusion matrix
 * @param se95 (type: bool) (default: 0) Report standard error for 95 confidence interval
 * @param boundary (type: short) (default: 1) Boundary for selecting the sample
 * @param homogeneous (type: bool) (default: 0) Only take regions with homogeneous boundary into account (for reference datasets only)
 * @param circular (type: bool) (default: 0) Use circular boundary
 * @return shared pointer to image object
 **/
std::shared_ptr<Jim> Jim::diff(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  /* std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(); */
  ImgRaster::diff(*imgWriter, app);
  return(imgWriter);
}

///Check for difference with reference image
/**
 * @param refImage reference image object
 * @param band (type: unsigned int) (default: 0) Input (reference) raster band. Optionally, you can define different bands for input and reference bands respectively: -b 1 -b 0.
 * @param rmse (type: bool) (default: 0) Report root mean squared error
 * @param reg (type: bool) (default: 0) Report linear regression (Input = c0+c1*Reference)
 * @param confusion (type: bool) (default: 0) Create confusion matrix (to std out)
 * @param class (type: std::string) List of class names.
 * @param reclass (type: short) List of class values (use same order as in classname option).
 * @param nodata (type: double) No data value(s) in input or reference dataset are ignored
 * @param mask (type: std::string) Use the first band of the specified file as a validity mask. Nodata values can be set with the option msknodata.
 * @param msknodata (type: double) (default: 0) Mask value(s) where image is invalid. Use negative value for valid data (example: use -t -1: if only -1 is valid value)
 * @param output (type: std::string) Output dataset (optional)
 * @param cmf (type: std::string) (default: ascii) Format for confusion matrix (ascii or latex)
 * @param cmo (type: std::string) Output file for confusion matrix
 * @param se95 (type: bool) (default: 0) Report standard error for 95 confidence interval
 * @param ct (type: std::string) Color table in ASCII format having 5 columns: id R G B ALFA (0: transparent, 255: solid).
 * @param commission (type: short) (default: 2) Value for commission errors: input label < reference label
 * @return shared pointer to output image object
 **/
CPLErr Jim::diff(std::shared_ptr<Jim> refImage,app::AppFactory& app){
  return(ImgRaster::diff(*refImage,app));
}
///validate a classification image based on a reference dataset
/**
 * @param reference (type: std::string) Reference (raster or vector) dataset
 * @param ln (type: std::string) Layer name(s) in sample. Leave empty to select all (for vector reference datasets only)
 * @param band (type: unsigned int) (default: 0) Input (reference) raster band. Optionally, you can define different bands for input and reference bands respectively: -b 1 -b 0.
 * @param confusion (type: bool) (default: 0) Create confusion matrix (to std out)
 * @param lref (type: std::string) (default: label) Attribute name of the reference label (for vector reference datasets only)
 * @param class (type: std::string) List of class names.
 * @param reclass (type: short) List of class values (use same order as in classname option).
 * @param nodata (type: double) No data value(s) in input or reference dataset are ignored
 * @param mask (type: std::string) Use the first band of the specified file as a validity mask. Nodata values can be set with the option msknodata.
 * @param msknodata (type: double) (default: 0) Mask value(s) where image is invalid. Use negative value for valid data (example: use -t -1: if only -1 is valid value)
 * @param output (type: std::string) Output dataset (optional)
 * @param f (type: std::string) (default: SQLite) OGR format for output vector
 * @param lclass (type: std::string) (default: class) Attribute name of the classified label
 * @param cmf (type: std::string) (default: ascii) Format for confusion matrix (ascii or latex)
 * @param cmo (type: std::string) Output file for confusion matrix
 * @param se95 (type: bool) (default: 0) Report standard error for 95 confidence interval
 * @param boundary (type: short) (default: 1) Boundary for selecting the sample
 * @param homogeneous (type: bool) (default: 0) Only take regions with homogeneous boundary into account (for reference datasets only)
 * @param circular (type: bool) (default: 0) Use circular boundary
 * @return CE_None if successful, else CE_Failure
 **/
CPLErr Jim::validate(app::AppFactory& app){
  return(ImgRaster::validate(app));
}


///supervised classification (train with extractImg/extractOgr)
/**
 * @param training (type: std::string) Training vector file. A single vector file contains all training features (must be set as: b0, b1, b2,...) for all classes (class numbers identified by label option). Use multiple training files for bootstrap aggregation (alternative to the bag and bsize options, where a random subset is taken from a single training file)
 * @param cv (type: unsigned short) (default: 0) N-fold cross validation mode
 * @param cmf (type: std::string) (default: ascii) Format for confusion matrix (ascii or latex)
 * @param tln (type: std::string) Training layer name(s)
 * @param class (type: std::string) List of class names.
 * @param reclass (type: short) List of class values (use same order as in class opt).
 * @param f (type: std::string) (default: SQLite) Output ogr format for active training sample
 * @param ct (type: std::string) Color table in ASCII format having 5 columns: id R G B ALFA (0: transparent, 255: solid)
 * @param label (type: std::string) (default: label) Attribute name for class label in training vector file.
 * @param prior (type: double) (default: 0) Prior probabilities for each class (e.g., -p 0.3 -p 0.3 -p 0.2 ). Used for input only (ignored for cross validation)
 * @param extent (type: std::string) Only classify within extent from polygons in vector file
 * @param mask (type: std::string) Only classify within specified mask. For raster mask, set nodata values with the option msknodata.
 * @param msknodata (type: short) (default: 0) Mask value(s) not to consider for classification. Values will be taken over in classification image.
 * @param nodata (type: unsigned short) (default: 0) Nodata value to put where image is masked as nodata
 * @param band (type: unsigned int) Band index (starting from 0, either use band option or use start to end)
 * @param startband (type: unsigned int) Start band sequence number
 * @param endband (type: unsigned int) End band sequence number
 * @param balance (type: unsigned int) (default: 0) Balance the input data to this number of samples for each class
 * @param min (type: unsigned int) (default: 0) If number of training pixels is less then min, do not take this class into account (0: consider all classes)
 * @param bag (type: unsigned short) (default: 1) Number of bootstrap aggregations
 * @param bagsize (type: int) (default: 100) Percentage of features used from available training features for each bootstrap aggregation (one size for all classes, or a different size for each class respectively
 * @param comb (type: unsigned short) (default: 0) How to combine bootstrap aggregation classifiers (0: sum rule, 1: product rule, 2: max rule). Also used to aggregate classes with rc option.
 * @param classbag (type: std::string) Output for each individual bootstrap aggregation
 * @param prob (type: std::string) Probability image.
 * @param priorimg (type: std::string) (default: ) Prior probability image (multi-band img with band for each class
 * @param offset (type: double) (default: 0) Offset value for each spectral band input features: refl[band]=(DN[band]-offset[band])/scale[band]
 * @param scale (type: double) (default: 0) Scale value for each spectral band input features: refl=(DN[band]-offset[band])/scale[band] (use 0 if scale min and max in each band to -1.0 and 1.0)
 * @param random (type: bool) (default: 1) Randomize training data for balancing and bagging
 * @return shared pointer to classified image object
 **/
std::shared_ptr<Jim> Jim::classify(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  ImgRaster::classify(*imgWriter, app);
  return(imgWriter);
}


// std::shared_ptr<Jim> Jim::classifySVM(app::AppFactory& app){
//   std::shared_ptr<Jim> imgWriter=Jim::createImg();
//   /* std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(); */
//   ImgRaster::classifySVM(*imgWriter, app);
//   return(imgWriter);
// }

// std::shared_ptr<Jim> Jim::classifyANN(app::AppFactory& app){
//   std::shared_ptr<Jim> imgWriter=Jim::createImg();
//   /* std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(); */
//   ImgRaster::classifyANN(*imgWriter, app);
//   return(imgWriter);
// }

///todo: extend for other types than char
std::shared_ptr<Jim> Jim::classifySML(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  ImgRaster::classifySML<unsigned char>(*imgWriter, app);
  return(imgWriter);
}

std::shared_ptr<Jim> Jim::classifySML(JimList& referenceReader, app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  ImgRaster::classifySML<unsigned char>(referenceReader, *imgWriter, app);
  return(imgWriter);
}

///supervised classification using support vector machine (train with extractImg/extractOgr)
/**
 * @param svmtype (type: std::string) (default: C_SVC) Type of SVM (C_SVC, nu_SVC,one_class, epsilon_SVR, nu_SVR)
 * @param kerneltype (type: std::string) (default: radial) Type of kernel function (linear,polynomial,radial,sigmoid)
 * @param kd (type: unsigned short) (default: 3) Degree in kernel function
 * @param coef0 (type: float) (default: 0) Coef0 in kernel function
 * @param nu (type: float) (default: 0.5) The parameter nu of nu_SVC, one_class SVM, and nu_SVR
 * @param eloss (type: float) (default: 0.1) The epsilon in loss function of epsilon_SVR
 * @param cache (type: int) (default: 100) Cache memory size in MB
 * @param etol (type: float) (default: 0.001) The tolerance of termination criterion
 * @param shrink (type: bool) (default: 0) Whether to use the shrinking heuristics
 * @param probest (type: bool) (default: 1) Whether to train a SVC or SVR model for probability estimates
 * @return shared pointer to output image object
 **/
// std::shared_ptr<Jim> Jim::svm(app::AppFactory& app){
//   std::shared_ptr<Jim> imgWriter=Jim::createImg();
//   /* std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(); */
//   ImgRaster::svm(*imgWriter, app);
//   return(imgWriter);
// }

///supervised classification using support artificial neural network (train with extractImg/extractOgr)
/**
 * @param nactive (type: unsigned int) (default: 1) number of active training points
 * @param nneuron (type: unsigned int) (default: 5) number of neurons in hidden layers in neural network (multiple hidden layers are set by defining multiple number of neurons: -n 15 -n 1, default is one hidden layer with 5 neurons)
 * @param connection (type: float) (default: 1) connection reate (default: 1.0 for a fully connected network)
 * @param weights (type: float) (default: 0) weights for neural network. Apply to fully connected network only, starting from first input neuron to last output neuron, including the bias neurons (last neuron in each but last layer)
 * @param learning (type: float) (default: 0.7) learning rate (default: 0.7)
 * @param maxit (type: unsigned int) (default: 500) number of maximum iterations (epoch) (default: 500)
 * @return shared pointer to output image object
 **/
// std::shared_ptr<Jim> Jim::ann(app::AppFactory& app){
//   std::shared_ptr<Jim> imgWriter=Jim::createImg();
//   /* std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(); */
//   ImgRaster::ann(*imgWriter, app);
//   return(imgWriter);
// }

///stretch Jim image and return stretched image as shared pointer
/**
 * @param nodata (type: unsigned short) (default: 0) Nodata value to put where image is masked as nodata
 * @param src_min (type: double) clip source below this minimum value
 * @param src_max (type: double) clip source above this maximum value
 * @param dst_min (type: double) mininum value in output image
 * @param dst_max (type: double) maximum value in output image
 * @param band (type: unsigned int) Band index (starting from 0, either use band option or use start to end)
 * @param eq (type: bool) set to True for histogram equalization
 * @param otype (type: std::string) (default: GDT_Unknown) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image
 * @return shared pointer to classified image object
 **/
std::shared_ptr<Jim> Jim::stretch(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=Jim::createImg();
  /* std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(); */
  ImgRaster::stretch(*imgWriter, app);
  return(imgWriter);
}

///Apply thresholds: set to no data if not within thresholds t1 and t2
std::shared_ptr<Jim> Jim::setThreshold(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
  ImgRaster::setThreshold(*imgWriter,app);
  return(imgWriter);
}

///get mask from a raster dataset
/**
 * @param min (type: double) Values smaller than min threshold(s) are masked as invalid. Use one threshold for each band
 * @param max (type: double) Values greater than max threshold(s) are masked as invalid. Use one threshold for each band
 * @param data (type: unsigned short) (default: 1) value(s) for valid pixels: between min and max
 * @param nodata (type: unsigned short) (default: 0) value(s) for invalid pixels: not between min and max
 * @param band (type: short) (default: 0) band(s) used for mask
 * @param operator (type: std::string) (default: OR) Operator: [AND,OR].
 * @param otype (type: std::string) (default: Byte) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image
 * @param ct (type: std::string) color table (file with 5 columns: id R G B ALFA (0: transparent, 255: solid)
 * @return shared pointer to classified image object
 **/
std::shared_ptr<Jim> Jim::getMask(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
  ImgRaster::getMask(*imgWriter,app);
  return(imgWriter);
}

///set mask to raster dataset
/**
 * @param vectormask (type: std::string) Vector mask dataset(s)
 * @param msknodata (type: int) (default: 1) Mask value(s) where image has nodata. Use one value for each mask, or multiple values for a single mask.
 * @param mskband (type: short) (default: 0) Mask band to read (0 indexed). Provide band for each mask.
 * @param nodata (type: double) (default: 0) nodata value to put in image if not valid
 * @param eoption special options controlling rasterization (ATTRIBUTE|CHUNKYSIZE|ALL_TOUCHED|BURN_VALUE_FROM|MERGE_ALG)
 * @param operator (type: char) (default: =) Operator: < = > !. Use operator for each msknodata option
 * @param otype (type: std::string) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image
 * @return shared pointer to classified image object
 **/
std::shared_ptr<Jim> Jim::setMask(VectorOgr& ogrReader, app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
  ImgRaster::setMask(ogrReader, *imgWriter, app);
  return(imgWriter);
}

std::shared_ptr<Jim> Jim::setMask(JimList& maskList, app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
  ImgList maskReader;
  for(int imask=0;imask<maskList.size();++imask){
    maskReader.pushImage(maskList.getImage(imask));
  }
  ImgRaster::setMask(maskReader,*imgWriter, app);
  return(imgWriter);
}

///reclass raster dataset
/**
 * @param mask (type: std::string) Mask image(s)
 * @param msknodata (type: unsigned short) (default: 1) Mask value(s) where image has nodata. Use one value for each mask, or multiple values for a single mask.
 * @param nodata (type: int) (default: 0) nodata value to put in image if not valid (0)
 * @param code (type: std::string) Recode text file (2 colums: from to)
 * @param class (type: std::string) list of classes to reclass (in combination with reclass option)
 * @param reclass (type: std::string) list of recoded classes (in combination with class option)
 * @param ct (type: std::string) color table (file with 5 columns: id R G B ALFA (0: transparent, 255: solid)
 * @param otype (type: std::string) (default: ) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image
 * @param band (type: unsigned short) (default: 0) band index(es) to replace (other bands are copied to output)
 * @param fname (type: std::string) (default: label) field name of the shape file to be replaced
 * @return shared pointer to output image object
 **/
std::shared_ptr<Jim> Jim::reclass(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
  ImgRaster::reclass(*imgWriter, app);
  return(imgWriter);
}

// std::shared_ptr<jiplib::Jim> Jim::labelConstrainedCCsMultiband(Jim &imgRaster, int ox, int oy, int oz, int r1, int r2){
//   try{
//    //if(nrOfBand()<=1){
//    //	std::string errorString="Error: number of bands must be larger than 1";
//    //	throw(errorString);
//    //}
//     int nc=nrOfBand();
//     IMAGE * imout = 0;
//     IMAGE * imse=imgRaster.getMIA();
//     IMAGE ** imap;
//     imap = (IMAGE **) malloc(this->nrOfBand()*sizeof(IMAGE **));
//     for(int iband=0;iband<nrOfBand();++iband)
//       imap[iband]=getMIA(iband);
//     imout =::labelccms(imap,this->nrOfBand(),imse,ox,oy,oz,r1,r2);
//     if (imout){
//       std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(imout);
//       imgWriter->copyGeoTransform(*this);
//       imgWriter->setProjection(getProjectionRef());
//       return(imgWriter);
//     }
//     else{
//       std::string errorString="Error: labelConstrainedCCsMultiband() function in MIA failed, returning NULL pointer";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(0);
//   }
//   catch(...){
//     return(0);
//   }
// }
