import React, { useState, useContext, useEffect } from "react";

import {spawn, kill} from 'react-native-childprocess'

import './App.css';

let cmdID;
 
export async function start(){
    cmdID = await spawn('/sbin/ping', ['google.com'], {
        pwd: "/",
        stdout: (output) => {
            console.log('>>>', output)
        }
    });
}
 
export async function stop(){
    kill(cmdID);
}


function App() {
  const [query, setQuery] = useState("");


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


      // const data = {
      //     content: content,
      //     images: commentImages
      // }

      // // console.log("start sending...", data);

      // const res = await repliesComment(props.postId, props.data._id, data);


      // if(res.data.success){
      //     // window.location.href = '/'; 
      //     // console.log("success");
      //     // await props.updateCommentCount();
      //     // socketRef.current.emit("ClientSendServer", res.data.comment);
      //     props.socket.emit("ClientSendServer-reply", props.data._id, res.data.comment);

      //     setContent("");
      //     setCommentImages([]);

      //     return;
      // }
  }

  return (
    <div className="homepage-class">
        <div className="text-input">
            <input 
                type="text" 
                placeholder="input query" 
                value = {query} 
                onChange = {(event) => handleChange(event)}
                onKeyDown={(e) => handleOnKeyDown(e)}
            ></input>
        </div>
    </div>
  );
}

export default App;
