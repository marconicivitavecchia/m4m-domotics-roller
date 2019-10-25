#!/usr/bin/env node

const fs = require('fs');

var content;

const filename = "PROGMEM";

console.log(`Processing ${filename}`);

function readFile(err, data) {
    if (err) {
        throw err;
    }
    content = data;

    // Invoke the next step here however you like
    processFile();          // Or put the next step in a function and invoke it
}

function writeFile(filename, content) {
    fs.writeFile(`${filename}.gen.c`, content, 'utf8', function (err) {
        if (err) throw err;
        console.log(`Saved ${filename}!`);
    });
}

function processFile() {
    console.log(content);
    writeFile(content);
}


// First I want to read the file
fs.readFile(`./${filename}.c.html`,'utf8', readFile);

