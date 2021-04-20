
const mem=new WebAssembly.Memory({initial:0xffff,maximum:0xffff});
const heap=new Uint8Array(mem.buffer);

let offset=0;
const readOffsets={
};
const imports = { 
		malloc: function(size){
			const ret=offset+Math.floor(size/4)*4;
				offset+=size;
				return ret;
		},
		fopen: function(filename,mode){
			const xhr=new XMLHttpRequest(filename);
			xhr.responseType='arraybuffer';
			xhr.onreadystatechange=function(){
				if(xhr.response){
					const fd= malloc(	xhr.response.byteLength);
					heap.set(xhr.response, fd);
					readOffsets[fd]=0;
					return fd;
				}
			}

		}
		fread:(dest, size, num, fd)=>{
			readOffsets[fd]+=size*num;
		}
};
