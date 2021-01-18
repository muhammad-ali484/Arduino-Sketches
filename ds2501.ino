#define data 6
#define rom_addr 0X33             // Rom address
#define mem_addr 0xF0             // Memory function command

byte rom_read_buff[8]={0};
byte rom_read_buff_com[8]={0};
byte eprom_status[9]={0};
byte crc_rec[1]={0};
byte eprom[32]={0};

void setup() {
  // put your setup code here, to run once:
 
   Serial.begin(9600);
  _init_(); 
  select();
}

void _init_(){                          
      pinMode(data,OUTPUT);                               //Sending reset pulse and receiving presence pulse
  digitalWrite(data,0);
  delayMicroseconds(480);           //Minimum delay for reset pulse is 200 microseconds
  digitalWrite(data,1);
  delayMicroseconds(15);
  Serial.println("Plz wait..");
  while(digitalRead(data)){}
  delayMicroseconds(20);           //wait for finishing of presence pulse 
  if(digitalRead(data)){
  Serial.println("ROM detected");   
  }
}

void select(){             //selecting rom function commands
  switch(rom_addr){
    case 0x33:
    write_byte(rom_addr);
    read_rom_();
    break;

    case 0xf0:
    write_byte(rom_addr);
    search_rom_();
    break;
  }//end switch

  switch(mem_addr){
    case 0xAA:
    Serial.print("CRC of command and address by master : ");
    
    Serial.println(CRC(00,CRC(00,CRC(0xAA,0))),HEX);    //CRC of command and address calculated by master. 
                                                        //Function calls will start from inner to outer
    write_byte(0xAA);
    write_byte(00);
    write_byte(00);
   mem_read();
    break;
    
    case 0xF0:
     Serial.print("CRC of command and address by master : ");
    Serial.println(CRC(00,CRC(00,CRC(0xF0,0))),HEX);    
    write_byte(0xF0);
    write_byte(00);
    write_byte(00);
    mem_read();
    break;
  }//end switch
}
void write_bits(byte bits){

    switch (bits){
    case 0:
    noInterrupts();
    digitalWrite(data,0);
    pinMode(data,OUTPUT);
    delayMicroseconds(60);
    digitalWrite(data,1);
    interrupts();
    delayMicroseconds(10);
    break;
    
    case 1:
    noInterrupts();
    digitalWrite(data,0);
    pinMode(data,OUTPUT);
    delayMicroseconds(7);
    digitalWrite(data,1);
    interrupts();
    delayMicroseconds(60);
    break;
   }//end switch
}


void write_byte(byte addr){                        
  for(byte i=0;i<8;i++){
  write_bits(bitRead(addr,i));
 }
}

byte read_bit_(){                        //generates host falling edge for reading
  byte r=0;
  noInterrupts();
  pinMode(data,OUTPUT);
  digitalWrite(data,0);
  delayMicroseconds(3); 
  pinMode(data,INPUT);    
  //digitalWrite(ONE_WIRE_IO_PIN,1);
  delayMicroseconds(10);
  r= digitalRead(data);
  interrupts();  
  delayMicroseconds(53);
  return r;  
}

void read_rom_(){
  byte crc=0;
  for(byte i=0;i<8;i++){
    rom_read_buff[7-i]=read_byte();
   crc= CRC(rom_read_buff[7-i],crc);       //Passing calculated crc value to continue calculation 
 }//end for

    Serial.println();
 for(byte i=0;i<8;i++)
    Serial.print(rom_read_buff[i],HEX);
    Serial.println();
    Serial.println(crc,HEX);
}

byte read_byte(){
  byte temp=0;     
  for(byte j=0;j<8;j++){
    temp|=(read_bit_()<<j);
    
    }//end for
    return temp;
}

void search_rom_(){
 for(byte i=0;i<8;i++){
//  data_to_hex(rom_read_buff,i);
//  data_to_hex(rom_read_buff_com,i);
  write_byte(rom_read_buff[i]);
  //CRC(rom_read_buff[7-i]);
   }//end for

   for(byte i=0;i<8;i++)
    Serial.print(rom_read_buff[i],HEX);
    Serial.println("");

  for(byte i=0;i<8;i++)
    Serial.print(rom_read_buff_com[i],HEX);
    Serial.println("");
}

byte CRC(byte input, byte crc){

byte lsb=0,outbit=0;

  for(byte i=0;i<8;i++){
    lsb=crc&0x01; 
    crc=crc>>1;
    outbit=(input>>i)&0x01;
    outbit=(outbit^lsb);
    
    if(outbit)
    crc=crc^0x8C;
   
  }//end for
 return crc;
 }

 void mem_read(){
  byte crc=0;
  crc_rec[0]=read_byte();             
  Serial.print("CRC of command and address from device : ");
  Serial.println(crc_rec[0],HEX);
for(byte j=0;j<4;j++){
  for(byte i=0;i<32;i++){
    eprom[i]=read_byte();
    Serial.print(eprom[i],HEX);
    Serial.print(" ");
    crc=CRC(eprom[i],crc);
  }
  Serial.println();
  Serial.println((char *) eprom);
}
   Serial.print("CRC of data from device : ");
  Serial.println(read_byte(),HEX);
   Serial.println(crc,HEX);

 }

void loop() {
  // put your main code here, to run repeatedly:

}
