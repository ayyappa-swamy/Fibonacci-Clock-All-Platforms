import time

firstFib = [1,1,2,3,5]

currentHours = time.localtime().tm_hour
currentMinutes = time.localtime().tm_min

print "\n\nHours: ", currentHours," Minutes: ", currentMinutes


def getFibonacciList(number, auxillaryList): #Auxillary list is to search for numbers which are of higher priority

	finalList = []
	index = len(auxillaryList) - 1
	sum = 0

	while index > -1 :
		sum += auxillaryList[index]
		if sum < number:
			finalList += [auxillaryList[index]]
		elif sum == number:
			finalList += [auxillaryList[index]]
			break
		else :
			sum -= auxillaryList[index]
		index -= 1

	if sum < number:
		return finalList+ getFibonacciList(number-sum, [x for x in firstFib if x not in auxillaryList])

	return finalList

hourList = getFibonacciList(currentHours,firstFib)
minList = getFibonacciList(currentMinutes/5, [x for x in firstFib if x not in hourList])

print "\nHours List: ",hourList
print "Min List: ", minList
