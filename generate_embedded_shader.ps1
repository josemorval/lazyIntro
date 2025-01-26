param (
    [string]$inputFile = "source.shader",
    [string]$outputHeader = "generated_header.h"
)

# Leer el contenido del archivo
$content = Get-Content $inputFile -Raw

# Escapar caracteres especiales para uso en un literal C
$contentEscaped = $content -replace "`r`n", "\n" -replace "`n", "\n" -replace '"', '\"'

# Crear el archivo de cabecera con el contenido
$headerContent = @"
#ifndef GENERATED_HEADER_H
#define GENERATED_HEADER_H

const char shader_content[] = R"raw(
$content
)raw";

#endif // GENERATED_HEADER_H
"@

# Escribir el contenido al archivo de cabecera
Set-Content $outputHeader -Value $headerContent
