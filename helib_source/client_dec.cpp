/* Copyright (C) 2020 IBM Corp.
 * This program is Licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. See accompanying LICENSE file.
 */

// This is a sample program for education purposes only.
// It implements a very simple homomorphic encryption based
// db search algorithm for demonstration purposes.

// This country lookup example is derived from the BGV database demo
// code originally written by Jack Crawford for a lunch and learn
// session at IBM Research (Hursley) in 2019.
// The original example code ships with HElib and can be found at
// https://github.com/homenc/HElib/tree/master/examples/BGV_database_lookup

#include <iostream>

#include <helib/helib.h>
#include <helib/EncryptedArray.h>
#include <helib/ArgMap.h>
#include <NTL/BasicThreadPool.h>

// Utility function to print polynomials
void printPoly(NTL::ZZX& poly)
{
  for (int i = NTL::deg(poly); i >= 0; i--) {
    std::cout << poly[i] << "x^" << i;
    if (i > 0)
      std::cout << " + ";
    else
      std::cout << "\n";
  }
}

// Utility function to read <K,V> CSV data from file
std::vector<std::pair<std::string, std::string>> read_csv(std::string filename)
{
  std::vector<std::pair<std::string, std::string>> dataset;
  std::ifstream data_file(filename);

  if (!data_file.is_open())
    throw std::runtime_error(
        "Error: This example failed trying to open the data file: " + filename +
        "\n           Please check this file exists and try again.");

  std::vector<std::string> row;
  std::string line, entry, temp;

  if (data_file.good()) {
    // Read each line of file
    while (std::getline(data_file, line)) {
      row.clear();
      std::stringstream ss(line);
      while (getline(ss, entry, ',')) {
        row.push_back(entry);
      }
      // Add key value pairs to dataset
      dataset.push_back(std::make_pair(row[0], row[1]));
    }
  }

  data_file.close();
  return dataset;
}

int main(int argc, char* argv[])
{
  /************ HElib boiler plate ************/

  // Note: The parameters have been chosen to provide a somewhat
  // faster running time with a non-realistic security level.
  // Do Not use these parameters in real applications.

  // Plaintext prime modulus
  unsigned long p = 131;
  // Cyclotomic polynomial - defines phi(m)
  unsigned long m = 130; // this will give 48 slots
  // Hensel lifting (default = 1)
  unsigned long r = 1;
  // Number of bits of the modulus chain
  unsigned long bits = 1000;
  // Number of columns of Key-Switching matrix (default = 2 or 3)
  unsigned long c = 2;
  // Size of NTL thread pool (default =1)
  unsigned long nthreads = 1;
  // input database file name
  std::string db_filename = "./dataset.csv";
  // debug output (default no debug output)
  bool debug = false;

  helib::ArgMap amap;
  amap.arg("m", m, "Cyclotomic polynomial ring");
  amap.arg("p", p, "Plaintext prime modulus");
  amap.arg("r", r, "Hensel lifting");
  amap.arg("bits", bits, "# of bits in the modulus chain");
  amap.arg("c", c, "# fo columns of Key-Switching matrix");
  amap.arg("nthreads", nthreads, "Size of NTL thread pool");
  amap.arg("db_filename",
           db_filename,
           "Qualified name for the database filename");
  amap.toggle().arg("-debug", debug, "Toggle debug output", "");
  amap.parse(argc, argv);

  // set NTL Thread pool size
  if (nthreads > 1)
    NTL::SetNumThreads(nthreads);

  // std::cout << "\n*********************************************************";
  // std::cout << "\n*           Privacy Preserving Search Example           *";
  // std::cout << "\n*           =================================           *";
  // std::cout << "\n*                                                       *";
  // std::cout << "\n* This is a sample program for education purposes only. *";
  // std::cout << "\n* It implements a very simple homomorphic encryption    *";
  // std::cout << "\n* based db search algorithm for demonstration purposes. *";
  // std::cout << "\n*                                                       *";
  // std::cout << "\n*********************************************************";
  // std::cout << "\n" << std::endl;




  // client va server giong nhau
  std::cout << "---Initialising HE Environment ... ";
  // Initialize context
  // This object will hold information about the algebra used for this scheme.
  std::cout << "\n Loading the Context ... ";
  
  HELIB_NTIMER_START(timer_Context);

  std::ifstream inContextFile;
  inContextFile.open("context.txt");
  
  // Read in the context from the file
  helib::Context context =
      helib::Context::readFromJSON(inContextFile);
  // Close the ifstream
  inContextFile.close();
            
  HELIB_NTIMER_STOP(timer_Context);


  // Secret key management
  std::cout << "\nLoading Secret Key ...";
  HELIB_NTIMER_START(timer_SecKey);
  std::ifstream inSecretKeyFile;
  inSecretKeyFile.open("seckey.txt");
  
  // Read in the secret key from the file
  helib::SecKey secret_key =
      helib::SecKey::readFromJSON(inSecretKeyFile, context);

  // Close the ifstream
  inSecretKeyFile.close();
  
  HELIB_NTIMER_STOP(timer_SecKey);




  // Compute key-switching matrices that we need
  HELIB_NTIMER_START(timer_SKM);
  helib::addSome1DMatrices(secret_key);
  HELIB_NTIMER_STOP(timer_SKM);

  // Load public key
  std::cout << "\nLoading Public Key ...";
  HELIB_NTIMER_START(timer_PubKey);

  std::ifstream inPublicKeyFile;
  inPublicKeyFile.open("pubkey.txt");

  // Read in the public key from the file
  helib::PubKey public_key =
      helib::PubKey::readFromJSON(inPublicKeyFile, context);


  // Close the ifstream
  inPublicKeyFile.close();
  

  HELIB_NTIMER_STOP(timer_PubKey);




  // Get the EncryptedArray of the context
  const helib::EncryptedArray& ea = context.getEA();

  // Print the context
  std::cout << std::endl;
  if (debug)
    context.printout();

  // // Print the security level
  // // Note: This will be negligible to improve performance time.
  // std::cout << "\n***Security Level: " << context.securityLevel()
  //           << " *** Negligible for this example ***" << std::endl;

  // Get the number of slot (phi(m))
  long nslots = ea.size();
  // std::cout << "\nNumber of slots: " << nslots << std::endl;









  // ------------------------------------------------------------ Client ----------------------------------------------------
  /************ Decrypt and print result ************/


  std::ifstream inCtxtFile;
  inCtxtFile.open("enc_result.txt", std::ios::in);
  

  // Read in the ctxt from the file
  helib::Ctxt value =
      helib::Ctxt::readFromJSON(inCtxtFile, public_key);

  inCtxtFile.close();




  HELIB_NTIMER_START(timer_DecryptQueryResult);
  helib::Ptxt<helib::BGV> plaintext_result(context);
  secret_key.Decrypt(plaintext_result, value);
  HELIB_NTIMER_STOP(timer_DecryptQueryResult);

  // Convert from ASCII to a string
  std::string string_result;
  for (long i = 0; i < plaintext_result.size(); ++i)
    string_result.push_back(static_cast<long>(plaintext_result[i]));


  // HELIB_NTIMER_STOP(timer_TotalQuery);

  // Print DB Query Timers
  if (debug) {
    helib::printNamedTimer(std::cout << std::endl, "timer_EncryptQuery");
    helib::printNamedTimer(std::cout, "timer_QuerySearch");
    helib::printNamedTimer(std::cout, "timer_DecryptQueryResult");
    std::cout << std::endl;
  }

  if (string_result.at(0) == 0x00) {
    string_result =
        "Country name not in the database."
        "\n*** Please make sure to enter the name of a European Country"
        "\n*** with the first letter in upper case.";
  }
  std::cout << "\nQuery result: " << string_result << std::endl;
  helib::printNamedTimer(std::cout, "timer_TotalQuery");

  return 0;
}
