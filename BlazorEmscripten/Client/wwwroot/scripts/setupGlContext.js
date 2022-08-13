setupContext = (id) => {
    var canvas = document.getElementById(id);
    var handle = GL.createContext(canvas, {
        antialias: false,
        depth: false
    });
    GL.makeContextCurrent(handle);
    console.log("setupContext done");
};