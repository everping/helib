import imp
from flask import Flask
import datetime
import subprocess
from flask import request
from flask import jsonify
import json
from flask_cors import CORS

x = datetime.datetime.now()
  
# Initializing flask app
app = Flask(__name__)
CORS(app)
  
# Route for seeing a data
@app.route('/enc')
def enc():
    # exec binary file to get output
    country_name = request.args.get('country', default = '*', type = str).strip()

    cmd = "/home/lab/Desktop/HElib_demo/service/utils/BGV_country_db_lookup_client_enc <<< " + country_name



    subprocess.call(['bash', '-c', cmd])


    context = ""
    enc_query = ""
    pubkey = ""
    data = {}
    
    f1 = open("./context.txt", "r")
    context = f1.read()
    f1.close()

    f2 = open("./enc_query.txt", "r")
    enc_query = f2.read()
    f2.close()
    
    f3 = open("./pubkey.txt", "r")
    pubkey = f3.read()
    f3.close()

    data = {
        'context': context,
        'enc_query': enc_query,
        'pubkey': pubkey
    }
    # return output to client
    return data





# Route for seeing a data
@app.route('/dec', methods=['POST'])
def dec():
    # exec binary file to get output
    data = request.get_json()

    f1 = open("./enc_result.txt", "w")
    f1.write(data["enc_result"])
    f1.close()

    cmd = "/home/lab/Desktop/HElib_demo/service/utils/BGV_country_db_lookup_client_dec" + " > ./log/log.txt"
    subprocess.call(['bash', '-c', cmd])

    str_result = open("./log/log.txt", "r").read().split("Query result: ")[-1].split("\u0000")[0]
    print(str_result)

    # return output to client
    return {
        "ptx_result": str_result
    }


      
# Running app
if __name__ == '__main__':
    app.run(debug=True, port = 9999)