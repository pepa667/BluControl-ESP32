import fs from 'fs';
import {minify} from 'minify';
import tryToCatch from 'try-to-catch';

const options =
{
    html: {
        removeAttributeQuotes: false,
        removeOptionalTags: false,
    },
};
const srcDir = './src';
const distDir = './dist';

function getFiles(dirPath, filesList = [])
{
    const files = fs.readdirSync(dirPath);

    files.forEach(function(file)
    {
        if (fs.statSync(dirPath + '/' + file).isDirectory())
        {
            filesList = getFiles(dirPath + '/' + file, filesList);
        }
        else
        {
            filesList.push(dirPath + '/' + file);
        }
    });
  
    return filesList;
}

if (fs.existsSync(distDir))
{
    console.log(`Cleaning ${distDir}...`);
    await fs.promises.rm(distDir, { recursive: true, force: true });
}
await fs.promises.mkdir(distDir, { recursive: true });

const filesList = getFiles(srcDir);
filesList.every(async function(file)
{
    const filePath = file.substring(srcDir.length + 1);
    const fileDirectory = filePath.substring(0, filePath.lastIndexOf('/'));
    console.log(`Processing ${filePath}...`);

    if (!fs.existsSync(distDir + '/' + fileDirectory))
    {
        await fs.promises.mkdir(distDir + '/' + fileDirectory, { recursive: true });
    }

    if (!filePath.endsWith('.html') && !filePath.endsWith('.css') && !filePath.endsWith('.js'))
    {
        await fs.promises.copyFile(file, distDir + '/' + filePath);
        return true;
    }

    const [error, data] = await tryToCatch(minify, file, options);

    if (error)
    {
        console.error(`Error while processing ${filePath}: ${error.message}`);
        return false;
    }
    else
    {
        await fs.promises.writeFile(distDir + '/' + filePath, data);
    }
    return true;
});