
from io import TextIOWrapper

def compareJson(file_json_ref: str, file_json : str, logs : TextIOWrapper, skipCompoX: str) -> bool: 
    #  Compare two json files line per line

    dash = '-' * 40
    logs.write(dash+'\n')
    logs.write('FILES\n') 
    logs.write(dash+'\n')
    logs.write('Filename reference = ' + file_json_ref+'\n')
    logs.write('Filename = ' + file_json+'\n\n')
 
    logs.write(dash+'\n')
    logs.write('DIFFERENCES\n') 
    logs.write(dash+'\n')

    verdict = True
    isSkipX = False
    x_key = '\"x\": '

    json_ref_lines = []
    with open(file_json_ref, 'r') as rjf:
        json_ref_lines = rjf.readlines()

    with open(file_json, 'r') as jf:
        for ref_line in json_ref_lines:
            json_line = jf.readline()
            if "version" in json_line and "version" in ref_line:
                continue
            if skipCompoX != "" and not(isSkipX):
                name_line = '\"nodeName\": \"' + skipCompoX + '\"'
                if name_line in json_line and name_line in ref_line:
                    isSkipX = True
            if isSkipX and x_key in json_line and x_key in ref_line:
                isSkipX = False
                continue
            if json_line != ref_line:
                logs.write(ref_line) 
                logs.write(json_line) 
                logs.write('\n')
                verdict = False

    return verdict
  




