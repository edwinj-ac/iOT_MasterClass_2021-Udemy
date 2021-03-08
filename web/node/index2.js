var mysql = require('mysql');
var mqtt = require('mqtt');

//Conexión con SQL desde el server
var con = mysql.createConnection({
  host: "spac3r.tk",
  user: "admin_cursoiot",
  password: "161616",
  database: "admin_cursoiot"
});

//Conexion con MQTT credenciales
var options = {
  port: 1883,
  host: 'cursoiot.ga',
  clientId: 'acces_control_server_' + Math.round(Math.random() * (0-10000) * - 1),
  username: 'web_client',
  password: '1111',
  keepalive: 60,
  reconnectPeriod: 1000,
  protocoloId: 'MQIsdp',
  clean: true,
  encoding: 'utf8'
};

//iniciar conexion
var client = mqtt.connect("mqtt://spac3r.tk", options);

//Se realiza la conexión
client.on('connect', function(){
  console.log("Conexión MQTT exitosa");
  client.subscribe('+/#', function (err){
    console.log("Subscripcion exitosa!");
  });
});

//Recibe mensaje del Broker
client.on('message', (topic, message) => {
  console.log('Receive message from topic：', topic, '->', message.toString());

  var topic_splitted = topic.split("/");
  var serial_number = topic_splitted[0];
  var query = topic_splitted[1];

  if(query == "access_query"){
    var rfid_number = message.toString();

    var query = "SELECT * FROM cards_habs WHERE cards_number = '"+ rfid_number +"' AND devices_serie = '"+ serial_number +"'"; // consulta
    con.query(query, function(err, result,fields){
      if(err) throw err;
    //  console.log(result);//result[] result[0].hab_name = nombre
      if(result.length  == 1){
        //GRANTED
        client.publish(serial_number + "/user_name", result[0].hab_name);
        client.publish(serial_number + "/command", "granted");
        console.log("Acceso permitido a ... " + result[0].hab_name );
        console.log(result[0].hab_id);

      }else{
        //REFUSED
        client.publish(serial_number + "/command", "refused");

      }
      //se hace la consulta

      var query = "INSERT INTO `traffic` (`traffic_hab_id`) VALUES (" + result[0].hab_id + ");"; // consulta
      con.query(query, function(err, result,fields){
        if(err) throw err;
        console.log("Ingreso registrado en TRAFFIC");
      });
    })

  }



})

con.connect(function(err){
  if(err) throw err; //evita que se trabe y muestra el error

//una vez conectados, podemos hacer consultas
  console.log("Conexión a MYSQL exitosa!!...");
  //se hace la consulta
  var query = "SELECT * FROM users WHERE 1"; // consulta
  con.query(query, function(err, result,fields){
    if(err) throw err;
    if(result.length>0){
      console.log(result);
    }
  })
});



//para mantener la sesión con mysql abierta
setInterval(function(){
  var query = 'SELECT 1 + 1 as result'; // entretener con una consulta simple

  con.query(query, function(err, result,fields){
    if(err) throw err;
  });
}, 5000); //cada 5 segundos se reconecta
