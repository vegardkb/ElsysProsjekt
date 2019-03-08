function timeStamp(nCycles, count){
  var time = Date.now() - (count*nCycles*9);
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

function tempError(resistance, thermnom, tempnorm, bcoef, aRead){
	return temp(resistance, thermnom, tempnorm, bcoef, 0, aRead);
}

function ph(aRead, aOffset, offset){
  return Math.round((aRead+aOffset + offset)*3.5*5/1024*10)/10;
}

function phError(aRead, offset){
	return ph(aRead, 0, offset);
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

function turbError(aRead){
	var turb = aRead*5/1024; //Convert to voltage
	turb = -1120.4*turb*turb + 5742.3*turb -4352.9; //Convert from voltage to NTU
	if(turb < 0){ //Handle the error
		turb = 0;
	}
	turb = Math.round(turb);
}

function cond(aRead){
  return aRead;
}

function condError(aRead){
	return aRead;
}

function Decoder(bytes, port){
	if(bytes.length == 3){
		var aRead = (bytes[0] << 8) + bytes[1];
		var payload = Array(1);
		switch(bytes[2]){
			case 0:
				payload[0] = {
					type: "TEMPERATURE",
					value: tempError(10030, 10000, 25, 3435, aRead),
					timeCreated: Date.now(),
				}
				break;
			case 1:
				payload[0] = {
					type: "PH",
					value: phError(bytes[6*i+1], 0.09),
					timeCreated: Date.now(),
				}
				break;
			case 2:
				payload[0] = {
					type: "TURBIDITY",
					value: turbError(aRead),
					timeCreated: Date.now(),
				}
				break;
			case 3:
				payload[0] = {
					type: "CONDUCTIVITY",
					value: condError(aRead),
					timeCreated: Date.now(),
				}
				break;
			
		}
		return {
			data: payload
		};
	}
	else{
		var N = bytes.length/6;

		var payload2 = Array(N*4);
		var time = Array(N);

		for(var i = 0; i < N; ++i){
		payload2[i*4] = {
			type: "TEMPERATURE",
			value: temp(10030, 10000, 25, 3435, 450, bytes[6*i]),
			timeCreated: timeStamp(bytes[6*i + 5], (N-bytes[6*i+4])),
		}
		payload2[i*4+1] = {
			type: "PH",
			value: ph(bytes[6*i+1], 300, 0.09),
			timeCreated: timeStamp(bytes[6*i + 5], (N-bytes[6*i+4])),
		}
		payload2[i*4+2] = {
			type: "TURBIDITY",
			value: turb(bytes[6*i+2], 768),
			timeCreated: timeStamp(bytes[6*i + 5], (N-bytes[6*i+4])),
		}
		payload2[i*4+3] = {
			type: "CONDUCTIVITY",
			value: cond(bytes[6*i+3]),
			timeCreated: timeStamp(bytes[6*i + 5], (N-bytes[6*i+4])),
		}
		}
		return {
		data: payload2
		};
	}
}