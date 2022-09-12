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
  std::string db_filename = "./countries_dataset.csv";
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



  // -------------------------------------------------------server part -----------------------------------------------

  
  /************ Read in the database ************/
  std::vector<std::pair<std::string, std::string>> country_db;
  try {
    country_db = read_csv(db_filename);
  } catch (std::runtime_error& e) {
    std::cerr << "\n" << e.what() << std::endl;
    exit(1);
  }

  // Convert strings into numerical vectors
  std::cout << "\n---Initializing the encrypted key,value pair database ("
            << country_db.size() << " entries)...";
  std::cout
      << "\nConverting strings to numeric representation into Ptxt objects ..."
      << std::endl;

  // Generating the Plain text representation of Country DB
  HELIB_NTIMER_START(timer_PtxtCountryDB);
  std::vector<std::pair<helib::Ptxt<helib::BGV>, helib::Ptxt<helib::BGV>>>
      country_db_ptxt;
  for (const auto& country_capital_pair : country_db) {
    if (debug) {
      std::cout << "\t\tname_addr_pair.first size = "
                << country_capital_pair.first.size() << " ("
                << country_capital_pair.first << ")"
                << "\tname_addr_pair.second size = "
                << country_capital_pair.second.size() << " ("
                << country_capital_pair.second << ")" << std::endl;
    }

    helib::Ptxt<helib::BGV> country(context);
    // std::cout << "\tname size = " << country.size() << std::endl;
    for (long i = 0; i < country_capital_pair.first.size(); ++i)
      country.at(i) = country_capital_pair.first[i];

    helib::Ptxt<helib::BGV> capital(context);
    for (long i = 0; i < country_capital_pair.second.size(); ++i)
      capital.at(i) = country_capital_pair.second[i];
    country_db_ptxt.emplace_back(std::move(country), std::move(capital));
  }
  HELIB_NTIMER_STOP(timer_PtxtCountryDB);




  // Encrypt the Country DB
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





  std::cout << "Encrypting the database..." << std::endl;
  HELIB_NTIMER_START(timer_CtxtCountryDB);
  std::vector<std::pair<helib::Ctxt, helib::Ctxt>> encrypted_country_db;
  for (const auto& country_capital_pair : country_db_ptxt) {
    helib::Ctxt encrypted_country(public_key);
    helib::Ctxt encrypted_capital(public_key);
    public_key.Encrypt(encrypted_country, country_capital_pair.first);
    public_key.Encrypt(encrypted_capital, country_capital_pair.second);
    encrypted_country_db.emplace_back(std::move(encrypted_country),
                                      std::move(encrypted_capital));
  }

  HELIB_NTIMER_STOP(timer_CtxtCountryDB);

  // Print DB Creation Timers
  if (debug) {
    helib::printNamedTimer(std::cout << std::endl, "timer_Context");
    helib::printNamedTimer(std::cout, "timer_Chain");
    helib::printNamedTimer(std::cout, "timer_SecKey");
    helib::printNamedTimer(std::cout, "timer_SKM");
    helib::printNamedTimer(std::cout, "timer_PubKey");
    helib::printNamedTimer(std::cout, "timer_PtxtCountryDB");
    helib::printNamedTimer(std::cout, "timer_CtxtCountryDB");
  }

  std::cout << "\nInitialization Completed - Ready for Queries" << std::endl;
  std::cout << "--------------------------------------------" << std::endl;





// ----------------------------------------------------------- Server part ----------------------------------------------------
// receive encrypted query
  /************ Perform the database search ************/

  std::cout << "Searching in the database" << std::endl;
  std::cout << "This may take few minutes ... " << std::endl;


  std::ifstream inCtxtFile;
  inCtxtFile.open("enc_query.txt", std::ios::in);
  

  // Read in the ctxt from the file
  helib::Ctxt query =
      helib::Ctxt::readFromJSON(inCtxtFile, public_key);

  inCtxtFile.close();



  HELIB_NTIMER_START(timer_QuerySearch);
  std::vector<helib::Ctxt> mask;
  mask.reserve(country_db.size());
  for (const auto& encrypted_pair : encrypted_country_db) {
    helib::Ctxt mask_entry = encrypted_pair.first; // Copy of database key
    mask_entry -= query;                           // Calculate the difference
    mask_entry.power(p - 1);                       // Fermat's little theorem
    mask_entry.negate();                           // Negate the ciphertext
    mask_entry.addConstant(NTL::ZZX(1));           // 1 - mask = 0 or 1
    // Create a vector of copies of the mask
    std::vector<helib::Ctxt> rotated_masks(ea.size(), mask_entry);
    for (int i = 1; i < rotated_masks.size(); i++)
      ea.rotate(rotated_masks[i], i);             // Rotate each of the masks
    totalProduct(mask_entry, rotated_masks);      // Multiply each of the masks
    mask_entry.multiplyBy(encrypted_pair.second); // multiply mask with values
    mask.push_back(mask_entry);
  }

  // Aggregate the results into a single ciphertext
  // Note: This code is for educational purposes and thus we try to refrain
  // from using the STL and do not use std::accumulate
  helib::Ctxt value = mask[0];
  for (int i = 1; i < mask.size(); i++)
    value += mask[i];

  HELIB_NTIMER_STOP(timer_QuerySearch);


  // output encrypted result, this encrypted query will be sent to client for decrypting

  std::ofstream outCtxtFile;
  outCtxtFile.open("enc_result.txt", std::ios::out);
  if (outCtxtFile.is_open()) {
    // Write the ctxt to a file
    value.writeToJSON(outCtxtFile);
    // Close the ofstream
    outCtxtFile.close();
  } else {
    throw std::runtime_error("Could not open file 'enc_result.txt'.");
  }


  return 0;
}
