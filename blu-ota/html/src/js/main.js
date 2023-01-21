const MAX_BUF_SIZE = 10240;

function appendContent(text, style)
{
    if (text == '')
        return;
    const contentDiv = document.getElementById('output_content');
    const brTag = contentDiv.innerHTML == '' ? '' : '<br>';
    contentDiv.innerHTML = `<div class="${style}">${text}</div>${brTag}${contentDiv.innerHTML}`;
}
function appendError(text)
{
    appendContent(text, 'error')
}
function appendLog(text)
{
    appendContent(text, 'log')
}
function appendMessage(text)
{
    appendContent(text, 'message')
}

function onUploadClick()
{
    const updFilfiles = document.getElementById('updFile');
    if (updFilfiles.files.length < 1)
    {
        return;
    }
    uploadUpdate(updFilfiles.files[0]);
}

function onResetClick()
{
    const req = new XMLHttpRequest();
    req.open('POST', '/reset_device', true);
    req.onreadystatechange = () =>
    {
        if (req.readyState === 4 && req.response == 'OK')
        {
            appendMessage('The device is now restarting.');
        }
        else if(req.readyState === 4)
        {
            appendLog(req.response);
        }
    }

    req.send();
}

function setProgressBar(perc)
{
    document.getElementById('progress_bar').className = 'loading';
    document.getElementById('progress_bar_percent').style.width = `${perc}%`;
}

function uploadBinary(binaryData, currentPos, size, callback)
{
    const dataLen = size - currentPos > MAX_BUF_SIZE ? MAX_BUF_SIZE : size - currentPos;
    const binaryArray = new Int8Array(dataLen);
    for (let i = 0; i < dataLen; i++)
    {
        if (i + currentPos >= binaryData.length)
        {
            appendError('Error while uploading the update. The package is bigger than expected.');
            return;
        }

        binaryArray[i] = binaryData[i + currentPos];
    }

    const req = new XMLHttpRequest();
    req.open('POST', '/send_ota_binary', true);
    req.onreadystatechange = () =>
    {
        if (req.readyState === 4 && req.response == 'OK')
        {
            setProgressBar((currentPos + dataLen) * 100.0 / binaryData.length);

            if (currentPos + dataLen >= size)
            {
                req.open('POST', '/end_ota', true);
                req.onreadystatechange = () =>
                {
                    if (req.readyState === 4 && req.response == 'OK')
                    {
                        if (currentPos + dataLen >= binaryData.length)
                        {
                            appendMessage('Update uploaded correctly.');
                        }
                        else
                        {
                            if (callback !== undefined)
                            {
                                callback(currentPos + dataLen);
                            }
                        }
                    }
                    else if (req.readyState === 4)
                    {
                        appendError('Error while finishing the update. Please try again before restarting the device.');
                        appendLog(req.response);
                    }
                }

                req.send();
            }
            else
            {
                uploadBinary(binaryData, currentPos + dataLen, size, callback);
            }
        }
        else if (req.readyState === 4)
        {
            appendError('Error while uploading the update. Please try again before restarting the device.');
            appendLog(req.response);
        }
    }

    req.send(binaryArray);
}

function uploadUpdate(file)
{
    if (!file)
    {
        return;
    }
    const reader = new FileReader();
    reader.addEventListener('load', (event) =>
    {
        if (file.name.endsWith('.updpkg'))
        {
            uploadUpdateBytes(event.target.result, 0, true, undefined);
        }
        else if(file.name.endsWith('.upd'))
        {
            uploadUpdateBytes(event.target.result, 0, false, undefined);
        }
        else
        {
            appendError('Please upload a .upd or .updpkg file.');
        }
    });

    reader.readAsArrayBuffer(file);
}

function uploadUpdateBytes(bytesArray, offset, isPackage, callback)
{
    const dataArray = new Int8Array(bytesArray);
    let requiredLen = offset + 1;
    if (dataArray.length < requiredLen)
    {
        appendError('Error: The package is corrupted/invalid');
        return;
    }

    if (isPackage)
    {
        const finishCallback = function(newOffset)
        {
            uploadUpdateBytes(bytesArray, newOffset, false, finishCallback);
        };
        uploadUpdateBytes(bytesArray, 0, false, finishCallback);
    }
    else
    {
        switch(dataArray[offset])
        {
            case 1:
                requiredLen += 1 + 7;
                if (dataArray.length < requiredLen)
                {
                    appendError('Error: The package is corrupted/invalid');
                    return;
                }
                break;
            default:
                appendError('Error: The package is corrupted/invalid or is not compatible with this version of BluControl');
                return;
        }
        const metadataArray = new Int8Array(requiredLen - offset);
        for (let i = 0; i < requiredLen - offset; i++)
        {
            metadataArray[i] = dataArray[offset + i];
        }
        const sizeBytes = new Int8Array(7);
        for (let i = 0; i < 7; i++)
        {
            sizeBytes[i] = metadataArray[i + 2];
        }
        const dataSize = parseInt(String.fromCharCode.apply(String, sizeBytes));

        setProgressBar(0);

        const req = new XMLHttpRequest();
        req.open('POST', '/set_ota_metadata', true);
        req.onreadystatechange = () =>
        {
            if (req.readyState === 4 && req.response == 'OK')
            {
                uploadBinary(dataArray, requiredLen, dataSize + requiredLen, callback);
            }
            else if(req.readyState === 4)
            {
                appendError('Error while setting the meta data. Please try again.');
                appendLog(req.response);
            }
        }

        req.send(metadataArray);
    }
}