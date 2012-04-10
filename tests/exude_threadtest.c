#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <err.h>

#include <exude.h>

#define ALLOCSIZE	4096

<<<<<<< HEAD
=======
int			run = 1;

>>>>>>> remove pthread_cancel
/*
 * Run a bunch of malloc/free in parallel. to see if there's any races present
 * in exude.
 */
static void *
childfn(void *arg)
{
	void *p;

<<<<<<< HEAD
	while (1) {
=======
	while (run) {
>>>>>>> remove pthread_cancel
		p = e_malloc(ALLOCSIZE);
		e_free(&p);
	}

	return (NULL);
}

int
main(int argc, char *argv[])
{
	pthread_t	children[3];
	int		i;

	exude_enable(0);
	exude_enable_threads();

	printf("making threads\n");
	for (i = 0; i < 3; i++)
<<<<<<< HEAD
		if (pthread_create(&children[i], NULL, childfn, NULL) != 0)
			err(1, "can't make thread %d", i);

	sleep(60);
	printf("reaping threads\n");
	for (i = 0; i < 3; i++) {
		if (pthread_cancel(children[i]) != 0)
			err(1, "can't cancel thread %d", i);
=======
		if (pthread_create(&children[i], NULL, childfn, NULL) == NULL)
			err(1, "can't make thread %d", i);

	sleep(60);
	run = 0;
	printf("reaping threads\n");
	for (i = 0; i < 3; i++) {
		if (pthread_join(children[i], NULL) != 0)
			err(1, "can't join thread %d", i);
>>>>>>> remove pthread_cancel
	}

	printf("done\n");
	exude_cleanup();

	return (0);
}
