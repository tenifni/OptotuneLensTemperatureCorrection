# OptotuneLensTemperatureCorrection
Version 1 entirely uses the temperature sensor in the Opotune EL-10-30 lens to do current compensation to maintain a stable focal position. Note that, however, the temperature sensoor (STTS2004) inside does not have high enough resolution and is not fast enough to sense heating up of the lens liquid (the center part of the lens gets heat up a lot by our 10-20W beamm). Instead, a recommended way is to do optical feedback + temperature stablization of the photodiode signal. 

Current code (V1) calculates correction current based on temperature reading and target focal point. 
