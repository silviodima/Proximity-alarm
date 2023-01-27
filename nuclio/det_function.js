const mqtt = require('mqtt')
const rest = require('restler')
const ifttt_key = "your_ifttt_key"
const url = 'mqtt://your-local-ip'

const options = {
    port: 1883,
    host: url,
    clientId: 'detector_01',
    username: 'guest',
    password: 'guest',
};

function sendFeedbackMqtt(q,msg){
    const client = mqtt.connect(url, options);
    client.on('connect', function() {
        client.publish(q, msg,{qos:1},function(){
            client.end();
        });             
    });        
}

function sendIFTTTmsg(){
    rest.post(
    "https://maker.ifttt.com/trigger/detection_trigger/json/with/key/"+ ifttt_key, 
    {
        data: {
        value1: "ALLARME"
        },
    }
)
.on("complete", function(data) {
    console.log("msg sent");
});

}

exports.handler = function(context, event) {

    var msg = event.body.toString();

    if(msg == "ALARM"){
        console.log(msg);
        sendIFTTTmsg();
    }
    context.callback("");
};