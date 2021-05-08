# Optotune Lens Temperature Corrector
Version 1 entirely uses the temperature sensor in the Opotune EL-10-30 lens to do current compensation to maintain a stable focal position. Note that, however, the temperature sensoor (STTS2004) inside does not have high enough resolution and is not fast enough to sense the heating up of the lens liquid (The center part of the lens, where most intense part of the Gaussian beam hits, gets heat up by our 10-20W beam and drifts significantly). Instead, a recommended way is to do optical feedback + temperature stablization of the photodiode signal (V2)

Current code here (V1) calculates correction current based on temperature reading and target focal point. 
