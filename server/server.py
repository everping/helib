from flask import Flask
import datetime
  
x = datetime.datetime.now()
  
# Initializing flask app
app = Flask(__name__)
  
  
# Route for seeing a data
@app.route('/search')
def get_time():
    # exec binary file to get output
    
    # return output to client
    return {
        'Name':"geek", 
        "Age":"22",
        "Date":x, 
        "programming":"python"
        }
  
      
# Running app
if __name__ == '__main__':
    app.run(debug=True)