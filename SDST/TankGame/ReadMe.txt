>>>> Known Issues <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

There are magic numbers in Asteroid.cpp (and hopefully only in there), didn't have enough time to move them out.
Sometime asteroids will ghost out, making them unhittable, I think I fixed this

>>>> How to Use <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	I - spawn asteroid 
	O - remove asteroid 
	T - slow time 
	P - toggle pause 
	N - respawn player after death 
	F1 - developer mode

Juice features: 
	Bullets penetrate asteroids exploding on the way, with different explosions for enter and exit,
		collision trajectory is considered
	Bullets change color and fade
	Particles that fade

>>>> Deep Learning <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

>>>> Thoughts on naming conventions:

My coding experience thus far: do some coding, find out that constants are all caps in existing code (mine are not), now everything looks ugly and confusing. To stay consistent with the existing code base, I rename all my constants to all caps, repeat the exact same process for globals, and member variables. In retrospect I should have looked both ways before crossing the street. Find out the conventions before starting to code. What would have been even better is having a clear style guide. I don't know if the the style guide was held back till the end of week 2 intentionally or not. Just so that we could experience the pain without one. Well I feel the pain and really want one now. I've grown used to adhering to a style guide and coding without one feels like not having showered for 2 weeks.

Conclusion: A clear style guide is very important, have one ready BEFORE the coding phase of the project.


>>>> Reinventing the wheel.

I know that this topic has been mentioned in some form in class but I just wanted to share my personal experience on it.

I am used to using C#, with its nice features, like managed memory, generic container objects and sorting algorithms ready to go. In Prof. Forseth's class we literally made asteroids in 1 hour.

Initially, doing these first assignments felt like reinventing the wheel, and programming with my hands tied behind my back. We are not allowed to use std or smart pointers or any third party lib. I find myself having to scratch my head even for basic tasks like how to store the vertices of an asteroid. Why should we have to solve these problems with arrays? It seems like every stackoverflow answer to array management is "don't do it, use std::vector instead", and every answer to [new] and [delete] is "you should not use them, use smart pointers instead".
But as I dig deeper into the assignment, and actually tackle these problems head on, I find myself learning a lot of knowledge that I would otherwise never touch. I never bothered with figuring out how container objects worked, I just took them for granted, since they magically worked. Only by playing with arrays at this low level have I come to appreciate all the nuances and thought that are needed to make them work. I now feel like these constraints that are imposed on us are very valuable. We are not reinventing the wheel to really use the wheel, but rather to learn how to make it. The journey is more important than the destination.
