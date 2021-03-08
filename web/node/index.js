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
  if(topic == "values"){
    //client.publish("led1", "pinpong ->" + message.toString()); //Enviamos mensajes al esp32
    //console.log("Respuesta enivada...");

    var msg = message.toString();
    var sp = msg.split(","); //funcion para indicar la sepración de los datos y pasarlo a un array
    var temp1 = sp[0];
    var temp2 = sp[1];
    var temp3 = sp[2];

    //se hace la consulta
    var query = "INSERT INTO `admin_cursoiot`.`data` (`data_temp1`, `data_temp2`, `data_temp3`) VALUES (" + temp1 + ", " + temp2 + ", " + temp3 + ");"; // consulta
    con.query(query, function(err, result,fields){
      if(err) throw err;
      console.log("Fila insertada correctamente");
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
