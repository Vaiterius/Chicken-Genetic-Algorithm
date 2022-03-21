# Chicken Evolution
## Description
As we know, chickens were never meant to be good flyers as they were adapted to spend more
time on the ground, though some breeds are known be significantly better flyers than others.
This program simulates the selective breeding of a species of chicken over time until the
goal of being long-distance flyers is reached, in this case, at least a distance of 1000
meters before tiring out.

Try tweaking the constants and see its effects!

## To know
`population_fitness`: on a scale of `0.0` to `1.0`; a higher number means closer to distance goal

`average_distance`: overall population's average distance traveled in a recorded flight

The `Chicken` object:
- `speed`: in meters per second
- `stamina`: amount of "fuel" a chicken has for flight
- `stamina_per_s`: stamina expended per second of flight
- `last_distance`: last recorded distance traveled

# The Genetic Algorithm
## What is it?
Based on Charles Darwin's natural selection process, the genetic algorithm models selecting
the fittest individuals of a population and producing offspring for the next generation.

## The Process
1) Establish initial population
2) Loop until goal is reached:

    a) Grade fitness of current generation's population

    b) Select fittest individuals and cull the weakest

    c) Breed them together to produce offspring

    d) Mutate randomly the offspring - most outcomes nonbeneficial
