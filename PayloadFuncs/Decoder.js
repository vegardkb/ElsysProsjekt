
function timeStamp(nCycles, nMeasurments, count){
  var newTime = Date.now();
  var time = newTime - (nMeasurments - count - 1)*(nCycles*9.96)*1000;
  return time;
}

function temp(resistance, thermnom, tempnorm, bcoef, minAnlValue, aRead){
  var res = ((aRead+minAnlValue)/1023)*5;
  res = (res*resistance)/(5-res);
  
  var A = 0.9122666410e-03, B = 2.477216773e-04, C = 2.050750481e-7;
  
  var temp = (1.0 / (A + B*Math.log(res) + C*Math.log(res)*Math.log(res)*Math.log(res)));  // Steinhart and Hart Equation. T  = 1 / {A + B[ln(R)] + C[ln(R)]^3}
  temp = temp - 273.15;
  return Math.round(temp*10)/10;
}

function ph(aRead, aOffset, offset){
  return Math.round((aRead+aOffset + offset)*3.5*5/1024*10)/10;
}


function turb(aRead, offset){
  var turb = (( aRead + offset)*5)/1024.0;//Convert to voltage
  turb = -1120.4*turb*turb + 5742.3*turb -4352.9; //Convert from voltage to NTU
  if(turb < 0){
    turb = 0;
  }
  turb = Math.round(turb);
  return turb;
}

function cond(aRead, temp){
	aRead*= 2;
	medianVoltage = aRead *3.3 / 1024.0;
	var compensationCoefficient=1.0+0.02*(temp-25.0);
	var compensationVolatge=medianVoltage/compensationCoefficient;
	var tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5;
	return tdsValue;
}


function Decoder(bytes, port){
	var N = bytes.length/6;

	var payload = Array(N*4);

	for(var i = 0; i < N; ++i){
		var time = timeStamp(bytes[6*i + 5], N, bytes[6*i+4]);
		var theTemp = temp(10030, 10000, 25, 3435, 510, bytes[6*i]);
		payload[i*4] = {
			type: "TEMPERATURE",
			value: theTemp,
			timeCreated: time,
		};
		payload[i*4+1] = {
			type: "PH",
			value: ph(bytes[6*i+1], 300, 0.09),
			timeCreated: time,
		};
		payload[i*4+2] = {
			type: "TURBIDITY",
			value: turb(bytes[6*i+2], 768),
			timeCreated: time,
		};
		payload[i*4+3] = {
			type: "CONDUCTIVITY",
			value: cond(bytes[6*i+3], theTemp),
			timeCreated: time,
		};
	}
	return {
	data: payload
	};
	
}