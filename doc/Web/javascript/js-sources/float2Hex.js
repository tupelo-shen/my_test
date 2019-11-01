function float2Hex(data) 
{
    if (data === undefined)     return "00000000";
    if (data === null)          return "00000000";
    if (parseFloat(data) === 0) return "00000000";
    
    var sign = "0";
    if (data < 0.0) {
        sign = "1";
        data = -data;
    }

    var mantissa = parseFloat(data).toString(2);
    var exponent = 0;
    
    if (mantissa.indexOf(".") === -1) {
        mantissa += ".0";
    }

    // var regex = /\((.+?)\)/g;
    var regex = /(?<=\(e)([-]|[+])\d+(?=\))/g;
    if(isIE && regex.exec()) {  
        exponent_str = mantissa.match(regex);
        if (exponent_str.substr(2,1) === "-") {
            exponent = parseInt(exponent_str,10) + 127;
        } else {
            return "7FC00000"; // NaN
        }
        
    }else {
        if (mantissa.substr(0, 1) === "0") {
            exponent = mantissa.indexOf(".") - mantissa.indexOf("1") + 127;
        } else {
            exponent = mantissa.indexOf(".") - 1 + 127;
        }
    }


    mantissa = mantissa.replace(".", "");
    mantissa = mantissa.substr(mantissa.indexOf("1")+1);
    var carry = mantissa.substr(23,1);

    if (mantissa.length > 23 ) {
        mantissa = mantissa.substr(0,23);
    } else {
        while (mantissa.length < 23){
            mantissa = mantissa +"0";
        }
    }

    var exp = parseFloat(exponent).toString(2);
    while (exp.length < 8) {
        exp = "0" + exp;
    }
    var numberFull = sign + exp + mantissa;

    var binaryStringWithCarry = numberFull;;
    var binaryStringWithCarry_1 = numberFull;
    if (carry === "1")
    {
        binaryStringWithCarry = addCarry(numberFull);
        binaryStringWithCarry_1 = addCarry_1(numberFull);
        if (binaryStringWithCarry_1 === binaryStringWithCarry)
        {
            console.log("Carry's result is equal.");
        }
    }

    var ret = str2Int(binaryStringWithCarry, 2).toString(16);
    return ret;
}

function addCarry_1(original)
{
    var str = (parseInt(original, 2) + 1).toString(2);
    var numZeroPadding = (str.length === 32) ? (0) : (32-str.length);
    rlt = str;
    for (var i = 0; i < numZeroPadding; i++)
    {
        rlt = "0" + rlt;
    }
    
    return rlt;
}

function addCarry(original)
{
    var retString = new Array();
    if (/[0-1]{32}/.exec(original))
    {
        for (var i = 0; i < 32; i++)
        {
            retString[i] = original.split("")[i];
        }
        for (var i = 31; i >= 0; i--)
        {
            if (retString[i] === "1")
            {
                retString[i] = "0";
            }
            else
            {
                retString[i] = "1";
                break;
            }
        }
    }

    return retString.join("");
}