function get_data_json() {
  $.ajax({
    url: "/data.json",
    type: "GET",
    dataType: "json",
    success: function (Jdata) {
      console.log("Get time data json success!");
      console.log(Jdata);
      $("#id_ttime").html(Jdata[0]["ttime_m"]+" 分 "+Jdata[0]["ttime_s"]+" 秒");
      $("#id_stime").html(Jdata[0]["stime_m"]+" 分 "+Jdata[0]["stime_s"]+" 秒");
      $("#id_nstate").html(Jdata[0]["nstate"] == "0" ? "STOP" : "START");
      $("#id_ftime").html(Jdata[0]["ftime"]);
      $("#id_ptime").html(Jdata[0]["ptime"]);
    },

    error: function () {
      console.log("Get time data json error!");
    }
  });
}