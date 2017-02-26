#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Object;

struct Class_Object
{
};

struct Object
{
	struct Class_Object *klass;
};

static struct Class_Object* Object_Class_Instance;

static void loadObject()
{
	Object_Class_Instance = malloc(sizeof(struct Class_Object));
}

static void initializeObject(struct Class_Object* klass)
{
}

static struct Object* initObject(struct Object* object)
{
	return object;
}

// Animal
struct Animal;

struct Class_Animal{
	struct Class_Object super;
	const char* (*emit_sound)(struct Animal*);
	size_t (*code)(struct Animal*);
};

static struct Class_Animal* Animal_Class_Instance;

struct Animal {
	struct Class_Animal* klass;
};

static size_t animalCode(struct Animal* animal)
{	
	return strlen(animal->klass->emit_sound(animal));
}

void loadAnimal()
{
	Animal_Class_Instance = malloc(sizeof(struct Class_Animal));
}

void initializeAnimal(struct Class_Animal* klass)
{
	initializeObject(klass);
	klass->code = animalCode;
}

struct Animal* initAnimal(struct Animal* animal)
{
	animal = initObject(animal);
	return animal;
}

// Dog

struct Class_Dog {
	struct Class_Animal super;
};

static struct Class_Dog* Dog_Class_Instance;

struct Dog
{
	struct Class_Dog* klass;
};

static const char* bark(struct Animal* dog)
{
	return "Auau";
}

static void loadDog()
{
	Dog_Class_Instance = malloc(sizeof(struct Class_Dog));
}

static void initializeDog(struct Class_Dog* klass)
{
	initializeAnimal(klass);
	klass->super.emit_sound = bark;
}

static struct Dog* newDog()
{
	struct Dog* dog = malloc(sizeof(struct Dog));
	dog->klass = Dog_Class_Instance;
	return dog;
}

static struct Dog* initDog(struct Dog* dog)
{
	dog = initAnimal(dog);
	return dog;
}

static int isDog(struct Object* obj)
{
	return obj->klass == Dog_Class_Instance;
}

// Cat

struct Class_Cat{
	struct Class_Animal super; 
};

static struct Class_Cat* Cat_Class_Instance;

struct Cat
{
	struct Class_Cat* klass;
	const char* _sound;
};

static const char* meow(struct Animal* animal)
{
	struct Cat* cat = (struct Cat*)animal;
	return cat->_sound;
}

static void loadCat()
{
	Cat_Class_Instance = malloc(sizeof(struct Class_Cat));
}

static void initializeCat(struct Class_Cat* klass)
{
	initializeAnimal(klass);
	klass->super.emit_sound = meow;
}

static size_t catCode(struct Animal* animal)
{
	return 42;
}

static struct Cat* newCat()
{
	struct Cat* cat = malloc(sizeof(struct Cat));
	cat->klass = Cat_Class_Instance;

	// override method from super
	cat->klass->super.code = catCode;
	return cat;
}

static struct Cat* initCat(struct Cat* cat, const char* sound)
{
	cat = initAnimal(cat);
	cat->_sound = sound;
	return cat;
}

static int isCat(struct Object* obj)
{
	return obj->klass == Cat_Class_Instance;
}

static void useAnimal(struct Animal* animal)
{
	printf("Animal Code: %lu\n", animal->klass->code(animal));

	if (isDog(animal)) {
		printf("Is Dog: ");
	}

	if (isCat(animal)) {
		printf("Is Cat: ");
	}

	printf("%s\n", animal->klass->emit_sound(animal));
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	loadObject();
	initializeObject(Object_Class_Instance);

	loadAnimal();
	initializeAnimal(Animal_Class_Instance);

	loadDog();
	initializeDog(Dog_Class_Instance);

	loadCat();
	initializeCat(Cat_Class_Instance);
	
	struct Animal* animals[] = {
		(struct Animal*)initDog(newDog()),
		(struct Animal*)initCat(newCat(),"123456"),
		(struct Animal*)initDog(newDog()),
		(struct Animal*)initCat(newCat(),"12345678"),
		(struct Animal*)initCat(newCat(),"123456789"),
		NULL};

	for (size_t i = 0; animals[i] != NULL; i++) {
		struct Animal* animal = animals[i];
		useAnimal(animal);
	}

	for (size_t i = 0; animals[i] != NULL; i++) {
	 	free(animals[i]);
	}

	return 0;
}
