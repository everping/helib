import React, { useState, useContext, useEffect } from "react";
import './App.css';
import axios from 'axios';
import { serviceURL, serverURL } from './context/constant';




function App() {
  const [query, setQuery] = useState("");
  const [result, setResult] = useState("");


  const handleChange = (event) => {
      // console.log(event.target.value);
      setQuery(event.target.value);
  }

  const handleOnKeyDown = (event) => {
      if (event.keyCode === 13) {
          submitQuery();
      }
  }



  const submitQuery = async () => {
      // get encrypted query json
    setResult("Searching ...")
    let country = query;

    let url = serviceURL + '/enc?country=' + country;
    let res = await axios.get(url);
    let data = res.data

    // console.log("check data", data)
    

    // send encrypted files to server
    url = serverURL + '/search'
    res = await axios.post(url, data);
    data = res.data
    // console.log(res)


    url = serviceURL + '/dec'
    res = await axios.post(url, data);
    
    // console.log(res)

    setResult("The capacity of " + query + " is: " + res.data.ptx_result)
  }

  return (
    <div className="homepage-class">
        <div className="text-input">
            <input 
                type="text" 
                placeholder="Input query" 
                value = {query} 
                onChange = {(event) => handleChange(event)}
                onKeyDown={(e) => handleOnKeyDown(e)}
            ></input>

            <div className="result">{result}</div>
        </div>
    </div>
  );
}

export default App;
