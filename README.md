# SQLito
Un proyecto para el curso de Base de Datos II de la Universidad Nacional de San Agustin.

## Descripción

Es un programa basado en Megatron que permite crear tablas, insertar datos, consultar datos y actualizar datos en una base de datos local con archivos estáticos. El programa no es Case Sensitive, por lo que puedes escribir `CREATE TABLE` o `create table` y el programa lo reconocerá, al igual que los nombres de las tablas y los campos.

## Participantes

- Fabricio Jesús Huaquisto Quispe

## Compilación

Primero debe de descargar el código fuente del proyecto. Puede hacerlo clonando el repositorio con el siguiente comando:

```bash
git clone https://github.com/fhocus/SQLito.git
```

Una vez descargado, debe de compilar el programa con el siguiente comando:

```bash
make
```

Este comando compilará el programa y generará un ejecutable llamado `SQLito.exe`.

## Uso

Puede ejecutar el programa con el siguiente comando:

```bash
./SQLito.exe
```

Al ejecutar el programa le aparecerá una bienvenida y una consola de prompt. Puedes escribir comandos en la consola para ejecutar los comandos de SQLito.

### Comandos

#### CREATE TABLE

El comando `CREATE TABLE` permite crear una tabla en la base de datos. El comando recibe un nombre de la tabla y una lista de campos y tipos separados por comas.

```sql
CREATE TABLE <table_name> SET <field_name> <field_type>, <field_name> <field_type>, ..., <field_name> <field_type>;
```

#### INSERT

El comando `INSERT` permite insertar datos en una tabla. El comando recibe el nombre de la tabla en la que se van a insertar los datos y una lista de campos y valores separados por comas.

```sql
INSERT <table_name> SET <field_name> <field_value>, <field_name> <field_value>, ..., <field_name> <field_value>;
```

#### UPDATE

El comando `UPDATE` permite actualizar datos en una tabla. El comando recibe el nombre de la tabla en la que se van a actualizar los datos, una lista de campos y valores separados por comas.

```sql
UPDATE <table_name> SET <field_name> <field_value>, <field_name> <field_value>, ..., <field_name> <field_value>;
```

El comando `UPDATE` también puede recibir un grupo de filtros que se aplicarán a los datos devueltos.

```sql
UPDATE <table_name> SET ... WHERE <field_name> <operator> <value> AND <field_name> <operator> <value> OR ... AND <field_name> <operator> <value>;
```

#### SELECT

El comando `SELECT` permite consultar datos de una tabla. El comando recibe el nombre de la tabla desde la que se van a consultar los datos y una lista de campos separados por comas.

```sql
SELECT <field_name>, <field_name>, ..., <field_name> FROM <table_name>;
```

El comando `SELECT` también puede recibir un grupo de filtros que se aplicarán a los datos devueltos.

```sql
SELECT ... FROM <table_name> WHERE <field_name> <operator> <value> AND <field_name> <operator> <value> OR ... AND <field_name> <operator> <value>;
```

Puede hacer JOIN entre tablas en el comando `SELECT` separando ambas tablas con un coma y consultando los campos de ambas tablas con el nombre de la tabla y el campo separados por puntos.

```sql
SELECT <table_name_1>.<field_name_1>, <table_name_2>_<field_name_2> FROM <table_name_1>, <table_name_2>.
```

Esto también funciona para los filtros.

Puede guardar el resultado de una consulta en una nueva tabla con el siguiente comando:

```sql
SELECT ... INTO <table_name> | <new_table_name>;
```

Esto guardará los datos devueltos en una nueva tabla que podrá consultar luego con otras consultas. Los campos de la nueva tabla deben ser declarados tendrán la combinación del nombre de la tabla y el nombre del campo.

```sql
<table_name>_<field_name>;
```

### Palabras reservadas

Las siguientes palabras son reservadas y no pueden ser usadas como nombres de tablas, campos o campos de datos:

#### Comandos

| Comando | Descripción |
|-----------|-----------|
| `CREATE` | Crea una tabla en la base de datos |
| `INSERT` | Inserta datos en una tabla |
| `UPDATE` | Actualiza datos en una tabla |
| `SELECT` | Consulta datos de una tabla |



#### Tipos de datos

| Tipo de datos | Descripción |
|-----------|-----------|
| `INT` | Indica que el campo es un número entero |
| `STR` | Indica que el campo es un texto |
| `BOOL` | Indica que el campo es un booleano |

#### Operadores

| Operador | Descripción |
|-----------|-----------|
| `=` | Indica que el valor de un campo es igual al valor dado |
| `!` | Indica que el valor de un campo es distinto al valor dado |
| `>` | Indica que el valor de un campo es mayor al valor dado |
| `<` | Indica que el valor de un campo es menor al valor dado |

#### Especiales

| Especial | Descripción |
|-----------|-----------|
| `TABLE` | Hace referencia a que se trabajará con tablas |
| `SET` | Indica que se van a insertar datos en una tabla |
| `WHERE` | Indica que se van a aplicar filtros a los datos devueltos |
| `INTO` | Indica que se va a guardar los datos devueltos en una nueva tabla |
| `\|` | Indica que se va a guardar los datos devueltos en una nueva tabla |

## Ejemplos

Proximamente.