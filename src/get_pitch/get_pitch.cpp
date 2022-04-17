/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 
Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version
Options:
    -m FLOAT, --vmaxnorm FLOAT  Umbral de el maximo de la autocorrelación normalizada [default: 0.5]
    -p FLOAT, --pot_min FLOAT  Umbral de potencia [default: -50]
    -1 FLOAT, --R10_min FLOAT  Umbral de autocorrelació a 0 [default: 0.4]
    -h, --help  Show this screen
    --version   Show the version of the project
Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();

  float vmaxnorm = std::stof(args["--vmaxnorm"].asString());
  float pot_min = std::stof(args["--pot_min"].asString());
  float R10_min = std::stof(args["--R10_min"].asString());



  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, vmaxnorm, pot_min, R10_min, PitchAnalyzer::HAMMING, 50, 500) ;

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping 
    float max_senyal = 0.0;
  for( unsigned int i=0; i<x.size();i++){
    if(x[i] > max_senyal){
      max_senyal=x[i];
    }
  }
  float th_cp=0.03*max_senyal;
  for(unsigned int i=0; i < x.size();i++){
    x[i] = x[i] / max_senyal;
    if(abs(x[i]) < th_cp){
      x[i]=0;
    }
    if( x[i] > th_cp){
      x[i] -=th_cp;
    }
    if( x[i] < -th_cp){
      x[i]+=th_cp;
    }
  }

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  std::vector<float> aux(f0);
  unsigned int j = 0;
  float maxim,minim;

  for(j = 2; j < aux.size() - 1; ++j) {
    minim = min(min(aux[j-1], aux[j]), aux[j+1]);
    maxim = max(max(aux[j-1], aux[j]), aux[j+1]);
    f0[j] = aux[j-1] + aux[j] + aux[j+1] - minim - maxim;
  }
  // Write f0 contour into the output file
// Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}