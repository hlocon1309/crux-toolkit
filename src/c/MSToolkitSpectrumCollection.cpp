/**
 * \file MSToolkitSpectrumCollection.cpp
 * AUTHOR: Chris Park
 * CREATE DATE: 28 June 2006
 * \brief Class to read spectra files using the MSToolkit library.
 */
#include "MSToolkitSpectrumCollection.h" 
#include "crux-utils.h"
#include "parameter.h"
#include "MSReader.h"
//#include "Spectrum.h"

/**
 * Instantiates a new spectrum_collection object from a filename. 
 * Does not parse file. 
 */
MSToolkitSpectrumCollection::MSToolkitSpectrumCollection(
  const char* filename   ///< The spectrum collection filename.
 ) : SpectrumCollection(filename){

}

/**
 * Parses all the spectra from file designated by the filename member
 * variable.
 * \returns True if the spectra are parsed successfully. False if otherwise.
 */
bool MSToolkitSpectrumCollection::parse() {

  // spectrum_collection has already been parsed
  if(is_parsed_){
    return false;
  }

  // get a list of scans to include if requested
  const char* range_string = get_string_parameter("scan-number");
  int first_scan = get_first_in_range_string(range_string);
  int last_scan = get_last_in_range_string(range_string);
  if( first_scan == -1 || last_scan == -1 ){
    carp(CARP_FATAL, "The scan number range '%s' is invalid. "
         "Must be of the form <first>-<last>.", range_string);
  }
  
  carp(CARP_DEBUG, "Using mstoolkit to parse spectra.");

  MSToolkit::MSReader* mst_reader = new MSToolkit::MSReader();
  MSToolkit::Spectrum* mst_spectrum = new MSToolkit::Spectrum();
  
  // only read ms2 scans
  mst_reader->setFilter(MSToolkit::MS2);
  // read first spectrum
  mst_reader->readFile(filename_.c_str(), *mst_spectrum);
  
  while(mst_spectrum->getScanNumber() != 0) {
    // is this a scan to include? if not skip it
    if( mst_spectrum->getScanNumber() < first_scan ){
      mst_reader->readFile(NULL, *mst_spectrum);
      continue;
    } 
    // are we past the last scan?
    if( mst_spectrum->getScanNumber() > last_scan ){
      break;
    }
    Spectrum* parsed_spectrum = new Spectrum();
    parsed_spectrum->parseMstoolkitSpectrum(mst_spectrum, filename_.c_str());
    
    this->addSpectrumToEnd(parsed_spectrum);
      
    mst_reader->readFile(NULL, *mst_spectrum);
  }
  delete mst_spectrum;
  delete mst_reader;
  
  return true;
}

/**
 * Parses a single spectrum from a spectrum_collection with first scan
 * number equal to first_scan.  Removes any existing information in
 * the given spectrum. 
 * \returns True if the spectrum was allocated, false on error.
 */
bool MSToolkitSpectrumCollection::getSpectrum(
  int first_scan,      ///< The first scan of the spectrum to retrieve -in
  Spectrum* spectrum   ///< Put the spectrum info here
  )
{
  carp(CARP_DEBUG, "Using mstoolkit to parse spectrum");
  MSToolkit::MSReader* mst_reader = new MSToolkit::MSReader();
  MSToolkit::Spectrum* mst_spectrum = new MSToolkit::Spectrum();
  bool parsed = false;

  mst_reader->readFile(filename_.c_str(), *mst_spectrum, first_scan);

  if(mst_spectrum->getScanNumber() != 0) {
    spectrum->parseMstoolkitSpectrum(mst_spectrum,
                                     filename_.c_str());
    parsed = true;
  } else {
    carp(CARP_ERROR, "Spectrum %d does not exist in file", first_scan);
    parsed = false;
  }
  delete mst_spectrum;
  delete mst_reader;
  return parsed;
}

/**
 * Parses a single spectrum from a spectrum_collection with first scan
 * number equal to first_scan.
 * \returns The spectrum data from file or NULL.
 */
Spectrum* MSToolkitSpectrumCollection::getSpectrum(
  int first_scan      ///< The first scan of the spectrum to retrieve -in
  )
{
  carp(CARP_DEBUG, "Using mstoolkit to parse spectrum");
  MSToolkit::MSReader* mst_reader = new MSToolkit::MSReader();
  MSToolkit::Spectrum* mst_spectrum = new MSToolkit::Spectrum();
  Spectrum* return_spec = NULL;  

  mst_reader->readFile(filename_.c_str(), *mst_spectrum, first_scan);

  if(mst_spectrum->getScanNumber() != 0) {
    return_spec = new Spectrum();
    return_spec->parseMstoolkitSpectrum(mst_spectrum,
                                        filename_.c_str()); 
  } else {
    carp(CARP_ERROR, "Spectrum %d does not exist in file", first_scan);
    return_spec = NULL;
  }
  delete mst_spectrum;
  delete mst_reader;
  return return_spec;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * End:
 */