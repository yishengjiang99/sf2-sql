const mem = new WebAssembly.Memory({ initial: 0xffff, maximum: 0xffff });
const heap = new Uint8Array(mem.buffer);

let offset = 0;
const readOffsets = {};
const imports = {
  malloc: function (size) {
    const ret = offset + Math.floor(size / 4) * 4;
    offset += size;
    return ret;
  },
  fopen: async function (filename, mode) {
    return await new Promise((resolve) => {
      const xhr = new XMLHttpRequest(filename);
      xhr.responseType = "arraybuffer";
      xhr.onreadystatechange = function () {
        if (xhr.response) {
          const fd = malloc(xhr.response.byteLength);
          heap.set(xhr.response, fd);
          readOffsets[fd] = 0;
          resolve(fd);
        }
      };
    });
  },
  fread: (dest, size, num, fd) => {
    readOffsets[fd] += size * num;
    new DataView(heap, dest, size, num).buffer.set(heap.slice(fd, size * num));
  },
};
fetch("index.stdalone.c.wasm")
  .then((response) => response.arrayBuffer())
  .then((bytes) =>
    WebAssembly.instantiate(bytes, {
      env: {
        imports,
        memory: mem,
      },
    })
  )
  .then((results) => {
    results.instance.exports.memread("http://www.nba.com");
  });
