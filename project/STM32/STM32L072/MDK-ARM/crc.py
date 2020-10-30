import sys
import zlib

file_path=sys.argv[1]
length_position=16
crc_position=20



def read_file():
    bin_file=open(file_path,mode="rb")
    data=bin_file.read()
    bin_file.close()
    return data

def change_name(file_name):
    point_pos=file_name.find('.bin')
    if point_pos==-1:
        return point_pos
    new_file_name=file_name.replace(".bin","_crc.bin")
    return new_file_name

def crc16_xmodem(dat):
    wcrc = 0
    for i in dat:
        c = i
        for j in range(8):
            treat = c & 0x80
            c <<= 1
            bcrc = (wcrc >> 8) & 0x80
            wcrc <<= 1
            wcrc = wcrc & 0xffff
            if (treat != bcrc):
                wcrc ^= 0x1021
    return wcrc

def data_add_crc_length(old_bin_data):  
    length=len(old_bin_data)
    print("data length is :",hex(length))

    new_data=bytearray(old_bin_data)


    length_bytes=length.to_bytes(length=4, byteorder='little', signed=True)
    for i in range(4):
        new_data[length_position+i]=length_bytes[i]

    crc16=crc16_xmodem(new_data)
    print("crc16 value is ：",hex(crc16))
    crc16_bytes = int(crc16).to_bytes(length=4, byteorder='little', signed=True)
    new_data.append(0)
    new_data.append(0)
    new_data.append(0)
    new_data.append(0)
    new_data[length]=crc16_bytes[1]
    new_data[length+1]=crc16_bytes[0]

    if crc16_xmodem(new_data)==0:
        print("calculate crc16 check ok!")
 
    return new_data



    

def write_file(new_file_name,new_bin_data):    
    new_file=open(new_file_name,mode='wb+')
    new_file.write(new_bin_data)
    new_file.close()




if __name__ == "__main__":
    old_bin_data=bytearray(read_file())
    new_bin_data=data_add_crc_length(old_bin_data)
    new_file_name=change_name(file_path)
    write_file(new_file_name,new_bin_data)
    print("complete!")
