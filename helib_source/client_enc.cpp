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
#include <fstream>

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
  std::cout << "\nInitializing the Context ... ";
  HELIB_NTIMER_START(timer_Context);
  helib::Context context = helib::ContextBuilder<helib::BGV>()
                               .m(m)
                               .p(p)
                               .r(r)
                               .bits(bits)
                               .c(c)
                               .build();
  HELIB_NTIMER_STOP(timer_Context);



  std::ofstream outContextFile;
  outContextFile.open("context.txt", std::ios::out);
  if (outContextFile.is_open()) {
    // Write the context to a file
    context.writeToJSON(outContextFile);
    // Close the ofstream
    outContextFile.close();
  } else {
    throw std::runtime_error("Could not open file 'context.txt'.");
  }






  // Secret key management
  std::cout << "\nCreating Secret Key ...";
  HELIB_NTIMER_START(timer_SecKey);
  // Create a secret key associated with the context
  helib::SecKey secret_key = helib::SecKey(context);
  // Generate the secret key
  secret_key.GenSecKey();


  std::ofstream outSecretKeyFile;
  outSecretKeyFile.open("seckey.txt", std::ios::out);
  if (outSecretKeyFile.is_open()) {
    // Write the secret key to a file
    secret_key.writeToJSON(outSecretKeyFile);
    // Close the ofstream
    outSecretKeyFile.close();
  } else {
    throw std::runtime_error("Could not open file 'seckey.txt'.");
  }

  HELIB_NTIMER_STOP(timer_SecKey);

  // Compute key-switching matrices that we need
  HELIB_NTIMER_START(timer_SKM);
  helib::addSome1DMatrices(secret_key);
  HELIB_NTIMER_STOP(timer_SKM);

  
  // Public key management
  // Set the secret key (upcast: FHESecKey is a subclass of FHEPubKey)
  std::cout << "\nCreating Public Key ...";
  HELIB_NTIMER_START(timer_PubKey);

  const helib::PubKey& public_key = secret_key;

  std::ofstream outPublicKeyFile;
  outPublicKeyFile.open("pubkey.txt", std::ios::out);
  if (outPublicKeyFile.is_open()) {
    // Write the public key to a file
    public_key.writeToJSON(outPublicKeyFile);
    // Close the ofstream
    outPublicKeyFile.close();
  } else {
    throw std::runtime_error("Could not open file 'pubkey.txt'.");
  }


  HELIB_NTIMER_STOP(timer_PubKey);

  // Get the EncryptedArray of the context
  const helib::EncryptedArray& ea = context.getEA();

  // Print the context
  std::cout << std::endl;
  if (debug)
    context.printout();

  // Print the security level
  // Note: This will be negligible to improve performance time.
  std::cout << "\n***Security Level: " << context.securityLevel()
            << " *** Negligible for this example ***" << std::endl;

  // Get the number of slot (phi(m))
  long nslots = ea.size();
  std::cout << "\nNumber of slots: " << nslots << std::endl;









  // ------------------------------------------------------- Client part -----------------------------------------
  // client encrypt , send ciphertext to server
  /** Create the query **/

  // Read in query from the command line
  std::string query_string;
  std::cout << "\nPlease enter the name of a patient: ";
  // std::cin >> query_string;
  std::getline(std::cin, query_string);
  

  HELIB_NTIMER_START(timer_TotalQuery);

  HELIB_NTIMER_START(timer_EncryptQuery);
  // Convert query to a numerical vector
  helib::Ptxt<helib::BGV> query_ptxt(context);
  for (long i = 0; i < query_string.size(); ++i)
    query_ptxt[i] = query_string[i];

  // Encrypt the query
  helib::Ctxt query(public_key);
  public_key.Encrypt(query, query_ptxt);
  HELIB_NTIMER_STOP(timer_EncryptQuery);


  // output encrypted query, this encrypted query will be sent to server for secure searching
  // std::ostream* str;

  // std::cout << "\n -------------- Encrypted query --------------\n";
  

  std::ofstream outCtxtFile;
  outCtxtFile.open("enc_query.txt", std::ios::out);
  if (outCtxtFile.is_open()) {
    // Write the ctxt to a file
    query.writeToJSON(outCtxtFile);
    // Close the ofstream
    outCtxtFile.close();
  } else {
    throw std::runtime_error("Could not open file 'enc_query.txt'.");
  }


  

  

  return 0;
}
