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
    fs.writeFile(filename, content, 'utf8', function (err) {
        if (err) throw err;
        console.log(`Saved ${filename}!`);
    });
}

function processFile() {
    //console.log(content);
    const regex1 = /<!-- (const char .*) -->/gm;
    const regex2 = /<!-- .* -->/gm;
    const regex = /(.*)/gm;

    //str.replace(/(blue)/g, "$1-red");

    let m;

    while ((m = regex1.exec(content)) !== null) {
        // This is necessary to avoid infinite loops with zero-width matches
        if (m.index === regex1.lastIndex) {
            regex1.lastIndex++;
        }

        // The result can be accessed through the `m`-variable.
        m.forEach((match, groupIndex) => {
            console.log(`Found match, group ${groupIndex}: ${match}`);
        });
    }



    writeFile(`${filename}.gen.c`, content);
}


// First I want to read the file
fs.readFile(`./${filename}.c.html`, 'utf8', readFile);

