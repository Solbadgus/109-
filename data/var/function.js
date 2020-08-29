function get_data_json() {
  //$("#id_nstate").html(Jdata[0]["nstate"]);
  //nstate_data = Jdata[0]["nstate"];
  var nstate_data;
  var nstate;
  $.ajax({
    url: "/data.json",
    type: "GET",
    dataType: "json",
    success: function (Jdata) {
	  console.log("Get time data json success!");
      console.log(Jdata);
      $("#id_ttime").html(Jdata[0]["ttime_m"]+" 分 "+Jdata[0]["ttime_s"]+" 秒");
      $("#id_stime").html(Jdata[0]["stime_m"]+" 分 "+Jdata[0]["stime_s"]+" 秒");
	  nstate_data = Jdata[0]["nstate"];
      $("#id_ftime").html(Jdata[0]["ftime"]);
      $("#id_ptime").html(Jdata[0]["ptime"]);
	  switch(nstate_data)
	  {
        case "0":nstate = $("#id_nstate").html("STOP");;break;
        case "1":nstate = $("#id_nstate").html("START");;break;
        case "2":nstate = $("#id_nstate").html("SETTING");;break;
        case "3":nstate = $("#id_nstate").html("TIME'S UP SO UP COUNT");;break ;
      }
     //ducument.getElementById("id_nstate").innerHTML = nstate ;
     //$("#id_nstate").html(nstate);
	  
    },

    error: function () {
      console.log("Get time data json error!");
    }
  });
  /*switch(nstate_data)
  {
    case "0":nstate = $("#id_nstate").html("STOP");;break;
    case "1":nstate = $("#id_nstate").html("START");;break;
    case "2":nstate = $("#id_nstate").html("SETTING");;break;
    case "3":nstate = $("#id_nstate").html("TIME'S UP SO UP COUNT");;break ;
  }
  //ducument.getElementById("id_nstate").innerHTML = nstate ;
  $("#id_nstate").html(nstate);*/
}