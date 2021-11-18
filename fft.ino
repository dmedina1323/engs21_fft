/* NOTE TO READER: THE FFT FUNCTION IN THIS PROGRAM WAS ENTIRELY CREATED BY ARDUINO USER abhilashpatel121. ALL CREDIT FOR THE ENTIRETY OF THAT FUNCTION GOES TO THEM. THE RELATED CODE AND PROGRAM IS HOSTED ON ARDUINO'S PUBLIC PROJECT HUB, AND CAN BE VIEWED AND DOWNLOADED AT THIS LINK: https://create.arduino.cc/projecthub/abhilashpatel121/easyfft-fast-fourier-transform-fft-for-arduino-9d2677

THE REMAINING CODE IS THE WORK OF AND BELONGS TO TEAM 23 (ENGS 21, 21F)
*/

/*********** Global Local Variables **************/
int photoResistor = A0;
int sampleSize = 256;
float f_peaks[5]; // top 5 frequencies peaks in descending order

/* The below variables were initially part of the FFT function. They've been made
 *  global here, so they can be used in the loop() function to determine whether a given
 *  frequency is viable to use or not.
 */
float out_r[128]; // real part of transform
int in_ps[128];   // used for indexing into frequency and amplitude arrays

/**************** function prototypes **************/
float FFT(int in[], int N,float Frequency);

/**************** setup **************/
void setup() {
  Serial.begin(250000);
  pinMode(2, OUTPUT);   // set pin 2 as an output pin
  digitalWrite(2, LOW); // turn of digital pin 2 to begin
}

/**************** main loop **************/
void loop() {
  
  //  unsigned long startTime = micros();
  digitalWrite(2, LOW); // begin each loop ensuring the digital pin 2 is off
  // function variables
  int N = 128;
  int data[N];
  

  // read in N values from the photoresistor
  for(int i = 0; i < N; i++){
    data[i] = analogRead(photoResistor);
    delayMicroseconds(7800);
  }

  // unsigned long afterArray = micros();


  /**************** FFT function call **********************/
  FFT(data,N,128);
  /********************************************************/

  // unsigned long afterFFT = micros();
  
  // ACTIVATE SOLENOID
  // Currently, the solenoid will activate when a peak frequency between 12 and 14 Hz is detected
  // The number 99 came from the following observation: when the program is accurately sensing a flashing light,
  // the top amplitude value is at least 100 units greater than the bottom amplitude
  if (f_peaks[0] >= 12.00 && f_peaks[0] <= 14.00 && (out_r[in_ps[0]] - out_r[in_ps[4]]) > 99) {
    digitalWrite(2,HIGH);
    delay(3000);
    digitalWrite(2, LOW);
  }
  
  
  // prints top 5 amplitudes and corresponding frequencies
  /*
  Serial.println("Amplitude\tFrequency");
  Serial.println("---------------------------");
  for (int i = 0; i < 5; i++) {
    Serial.print(out_r[in_ps[i]]);
    Serial.print("\t\t");
    Serial.println(f_peaks[i]);
  }
  Serial.println("----------------------------\n\n");
  */
  
  //  uncomment the below block for information about run times
  //  you will also need to uncomment the aboves lines of code that call the micros() function

  /*
  // calculate times
  unsigned long valueReadTime = afterArray - startTime;
  unsigned long FFTTime = afterFFT - startTime;
  Serial.print(valueReadTime); Serial.print(", "); Serial.println(FFTTime);

  // convert to milliseconds
  valueReadTime = valueReadTime/1000;
  programTime = programTime/1000;

  // print time information
  Serial.print("It took "); Serial.print(valueReadTime); Serial.println(" milliseconds to read in analog values.");
  Serial.print("The total runtime of the program was "); Serial.print(programTime); Serial.println(" milliseconds.");
  */

  delay(100);
}

/************************** FFT **************************/
/*
Code to perform FFT on arduino,
setup:
paste sine_data [91] at top of program [global variable], paste FFT function at end of program
Term:
1. in[]     : Data array, 
2. N        : Number of sample (recommended sample size 2,4,8,16,32,64,128...)
3. Frequency: sampling frequency required as input (Hz)

If sample size is not in power of 2 it will be clipped to lower side of number. 
i.e, for 150 number of samples, code will consider first 128 sample, remaining sample  will be omitted.
For Arduino nano, FFT of more than 128 sample not possible due to mamory limitation (64 recomended)
For higher Number of sample may arise Mamory related issue,
Code by ABHILASH
Contact: abhilashpatel121@gmail.com 
Documentation:https://www.instructables.com/member/abhilash_patel/instructables/
2/3/2021: change data type of N from float to int for >=256 samples
*/
float FFT(int in[],int N,float Frequency)
{

  unsigned int data[13]={1,2,4,8,16,32,64,128,256,512,1024,2048};
  int a,c1,f,o,x;
  a=N;  
                                 
  for(int i=0;i<12;i++) {                 //calculating the levels
    if(data[i]<=a) {
      o=i;
    } 
  }
  
  float out_im[data[o]]={};  //imaginory part of transform
  x=0;  
  for(int b=0;b<o;b++) {                     // bit reversal
    c1=data[b];
    f=data[o]/(c1+c1);
    
    for(int j=0;j<c1;j++) { 
       x=x+1;
       in_ps[x]=in_ps[j]+f;
    }
    
  }

 
  for(int i=0;i<data[o];i++) {            // update input array as per bit reverse order
    
    if (in_ps[i] < a){
      out_r[i]=in[in_ps[i]];
    }
    
    if(in_ps[i] > a) {
      out_r[i]=in[in_ps[i]-a];
    }
    
  }
  
  int i10,i11,n1;
  float e,c,s,tr,ti;
  float rad = 0;

  /*************** FFT Process **************/
  for(int i=0;i<o;i++) {
    
    i10=data[i];              // overall values of sine/cosine  :
    i11=data[o]/data[i+1];    // loop with similar sine cosine:
    e=360/data[i+1];
    e=0-e;
    n1=0;

    for(int j=0;j<i10;j++) {
      
      rad = (e*j)*(PI/180); // convert to radians
      c = cos(rad);
      s = sin(rad);    
      n1=j;
          
      for(int k=0;k<i11;k++) {
        
       tr=c*out_r[i10+n1]-s*out_im[i10+n1];
       ti=s*out_r[i10+n1]+c*out_im[i10+n1];

       out_r[n1+i10]=out_r[n1]-tr;
       out_r[n1]=out_r[n1]+tr;

       out_im[n1+i10]=out_im[n1]-ti;
       out_im[n1]=out_im[n1]+ti;          

       n1=n1+i10+i10;
      }       
    }
  }

  /*
  for(int i=0;i<data[o];i++)
  {
  Serial.print(out_r[i]);
  Serial.print("\t");                                     // un comment to print RAW o/p    
  Serial.print(out_im[i]); Serial.println("i");      
  }
  */


  //---> here onward out_r contains amplitude and our_in conntains frequency (Hz)
  for(int i=0;i<data[o-1];i++) {               // getting amplitude from compex number
      
    out_r[i]=sqrt(out_r[i]*out_r[i]+out_im[i]*out_im[i]); // to  increase the speed delete sqrt
    out_im[i]=i*Frequency/N;
  }

  x=0;       // peak detection
  for(int i=1;i<data[o-1]-1;i++) {
    
    if(out_r[i]>out_r[i-1] && out_r[i]>out_r[i+1])  {
      in_ps[x]=i;    //in_ps array used for storage of peak number
      x=x+1;
    }
    
  }

  s=0;
  c=0;
  for(int i=0;i<x;i++) {         // re arraange as per magnitude
  
    for(int j=c;j<x;j++) {
      
      // bubble sort indices by magnitude - DM
      // out_r array contains magnitude - DM
      
      if(out_r[in_ps[i]]<out_r[in_ps[j]]) {
        
        s=in_ps[i];
        in_ps[i]=in_ps[j];
        in_ps[j]=s;
      }
     }
     c=c+1;
     
    }


    for(int i=0;i<5;i++) {     // updating f_peak array (global variable) with descending order
      f_peaks[i] = out_im[in_ps[i]];
    }
}
