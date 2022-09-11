from flask import Flask
import datetime
import subprocess
from flask import request


x = datetime.datetime.now()
  
# Initializing flask app
app = Flask(__name__)
  
  
# Route for seeing a data
@app.route('/enc')
def search():
    # exec binary file to get output
    country_name = request.args.get('country', default = '*', type = str).strip()

    cmd = "/BGV_country_db_lookup db_filename=./bgv_country_db_lookup/countries_dataset.csv <<< " + country_name + " > ./log/log.txt"



    subprocess.call(['bash', '-c', cmd])
    search_res = open("./log/log.txt", "r").read().split("Query result: ")[-1].split("\u0000")[0]

    # return output to client
    return {
        'search_result': search_res
    }
  
      
# Running app
if __name__ == '__main__':
    app.run(debug=True, port = 9999)