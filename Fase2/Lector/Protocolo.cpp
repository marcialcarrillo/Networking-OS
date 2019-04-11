Buzon:
	   Types
			1 ack constratista al lector. Informar que ya termino 
			2 ack emisor al lector. Informar que ya recibio el nombre del archivo
			3 ack emisor al contratista. Informar que termino de escribit el archivo
			4 Comienza la comunicacion con el Emisor, contiene el nombre, id y total de paquetes
			5 ack Lector al emisor. Informa que ya no hay mas imagenes
			2019-2xxx // Cada contratista tiene un identificador unico(inicia en 2019)
Lector:
		Ubica las imagenes
		Crea un contratista(proceso) por imagen(A lo mas 2) con un identificador (inicia en 2019)
			Espera un ack(1) del contratista para crear otro
		
		
Contratista:
		Envia el nombre del archivo y el identificador al emisor
		Espera un ack(2) del emisor
		Envia paquetes de 128bytes al emisor
		Se envia un ultimo paquete informando que se termino la imagen 	
		Espera un ack(3)
		Envia un ack(1) al lector informando que ya termino

Emisor:
	Recibe un paquete tipo 4
	Envia un ack(2)
	Escribe la imagen
	Envia un ack(3)
	
	
