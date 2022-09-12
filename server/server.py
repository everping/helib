from flask import Flask
import datetime
import subprocess
from flask import request
from flask_cors import CORS
from flask import jsonify


x = datetime.datetime.now()
  
# Initializing flask app
app = Flask(__name__)
CORS(app)
  
# Route for seeing a data
@app.route('/search', methods=['POST'])
def search():

    data = request.get_json()

    
    f1 = open("./context.txt", "w")
    f1.write(data["context"])
    f1.close()

    f2 = open("./enc_query.txt", "w")
    f2.write(data["enc_query"])
    f2.close()

    f3 = open("./pubkey.txt", "w")
    f3.write(data["pubkey"])
    f3.close()


    # exec binary file to get output
    cmd = "/home/lab/Desktop/HElib_demo/server/utils/BGV_country_db_lookup_server db_filename=/home/lab/Desktop/HElib_demo/server/utils/countries_dataset.csv"
    subprocess.call(['bash', '-c', cmd])
    

    f4 = open("./enc_result.txt", "r")
    enc_result = f4.read()
    f4.close()
    

    # return output to client
    return {
        'enc_result': enc_result
    }
  
      
# Running app
if __name__ == '__main__':
    app.run(debug=True)